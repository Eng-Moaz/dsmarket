#include "Supermarket.h"
#include <sstream>
#include <iomanip>

static string money(double value) {
    ostringstream out;
    out << fixed << setprecision(2) << value;
    return out.str();
}

Supermarket::Supermarket() : serviceLine(20), checkoutLane(20), restockQueue(20) {
    nextCustomerId = 1;
    clock = 0;
    totalSales = 0;
    itemsSold = 0;
    customersServed = 0;
}

void Supermarket::loadSampleData() {
    products.insert({101, "Milk", 25.50, 40});
    products.insert({205, "Bread", 10.00, 60});
    products.insert({150, "Eggs (12)", 65.00, 25});
    products.insert({330, "Rice 1kg", 42.75, 30});
    products.insert({120, "Cheese", 90.00, 4});
    products.insert({410, "Apples 1kg", 35.00, 50});
    products.insert({275, "Orange Juice", 30.25, 3});
    products.insert({180, "Chicken 1kg", 160.00, 15});
    rebuildTree();
}

void Supermarket::rebuildTree() {
    sortedProducts.clear();
    vector<Product> all = products.getAll();
    for (int i = 0; i < (int)all.size(); i++)
        sortedProducts.insert(all[i]);
}

// ---------------- Products ----------------

string Supermarket::addProduct(int id, string name, double price, int quantity) {
    if (id <= 0 || price < 0 || quantity < 0)
        return "Invalid product data.";

    Product p = {id, name, price, quantity};
    if (!products.insert(p))
        return "A product with ID " + to_string(id) + " already exists.";

    sortedProducts.insert(p);
    undoStack.push({"ADD_PRODUCT", p, 0});
    return "Product '" + name + "' added.";
}

string Supermarket::deleteProduct(int id) {
    Product* p = products.search(id);
    if (p == nullptr)
        return "Product not found.";

    Product copy = *p;
    products.remove(id);
    rebuildTree();
    undoStack.push({"DELETE_PRODUCT", copy, 0});
    return "Product '" + copy.name + "' deleted.";
}

string Supermarket::updatePrice(int id, double newPrice) {
    Product* p = products.search(id);
    if (p == nullptr)
        return "Product not found.";
    if (newPrice < 0)
        return "Price can't be negative.";

    undoStack.push({"UPDATE_PRODUCT", *p, 0});
    products.updatePrice(id, newPrice);
    rebuildTree();
    return "Price of '" + p->name + "' updated to " + money(newPrice) + ".";
}

string Supermarket::updateQuantity(int id, int newQuantity) {
    Product* p = products.search(id);
    if (p == nullptr)
        return "Product not found.";
    if (newQuantity < 0)
        return "Quantity can't be negative.";

    undoStack.push({"UPDATE_PRODUCT", *p, 0});
    products.updateQuantity(id, newQuantity);
    rebuildTree();
    return "Quantity of '" + p->name + "' updated to " + to_string(newQuantity) + ".";
}

Product* Supermarket::findProduct(int id) {
    return products.search(id);
}

Product* Supermarket::findProductInTree(int id) {
    return sortedProducts.search(id);
}

vector<Product> Supermarket::getAllProducts() {
    return products.getAll();
}

vector<Product> Supermarket::getSortedProducts() {
    return sortedProducts.inorder();
}

// ---------------- Customer service line ----------------

string Supermarket::addToServiceLine(string name) {
    if (serviceLine.isFull())
        return "The service desk is full, please come back later.";

    Customer c = {nextCustomerId++, name, 1, clock++};
    serviceLine.enqueue(c);
    return name + " took ticket #" + to_string(c.id) + ".";
}

string Supermarket::serveServiceLine() {
    Customer c;
    if (!serviceLine.dequeue(c))
        return "Nobody is waiting.";

    customersServed++;
    return "Now serving " + c.name + " (ticket #" + to_string(c.id) + ").";
}

vector<Customer> Supermarket::getServiceLine() {
    return serviceLine.getAll();
}

string Supermarket::serviceLineStatus() {
    if (serviceLine.isEmpty())
        return "Service line is empty.";

    string status = to_string(serviceLine.size()) + "/" + to_string(serviceLine.getCapacity()) + " waiting";
    status += " | Front: " + serviceLine.front().name;
    status += " | Back: " + serviceLine.back().name;
    if (serviceLine.isFull())
        status += " | FULL";
    return status;
}

// ---------------- Checkout lane ----------------

string Supermarket::addToCheckoutLane(string name, int priority) {
    if (priority < 1 || priority > 3)
        return "Priority must be 1, 2 or 3.";
    if (checkoutLane.isFull())
        return "The checkout line is full.";

    Customer c = {nextCustomerId++, name, priority, clock++};
    checkoutLane.insert(c);
    return name + " joined the checkout line as " + priorityName(priority) + ".";
}

string Supermarket::serveCheckoutLane() {
    Customer c;
    if (!checkoutLane.extractTop(c))
        return "No customers waiting.";

    customersServed++;
    return "Served " + c.name + " [" + priorityName(c.priority) + "].";
}

string Supermarket::peekCheckoutLane() {
    if (checkoutLane.isEmpty())
        return "No customers waiting.";

    Customer c = checkoutLane.peek();
    return "Next: " + c.name + " [" + priorityName(c.priority) + "]";
}

string Supermarket::changePriority(int customerId, int newPriority) {
    if (newPriority < 1 || newPriority > 3)
        return "Priority must be 1, 2 or 3.";
    if (!checkoutLane.updatePriority(customerId, newPriority))
        return "Customer not found in the checkout line.";
    return "Priority updated to " + priorityName(newPriority) + ".";
}

vector<Customer> Supermarket::getCheckoutLane() {
    return checkoutLane.getAll();
}

// ---------------- Shopping cart ----------------

string Supermarket::addToCart(int productId, int quantity) {
    Product* p = products.search(productId);
    if (p == nullptr)
        return "Product not found.";
    if (quantity <= 0)
        return "Quantity must be more than zero.";
    if (p->quantity < quantity)
        return "Not enough stock. Only " + to_string(p->quantity) + " left.";

    cart.add({p->id, p->name, p->price, quantity});
    p->quantity -= quantity;
    rebuildTree();
    undoStack.push({"ADD_TO_CART", *p, quantity});
    return "Added " + to_string(quantity) + " x " + p->name + " to the cart.";
}

string Supermarket::removeFromCart(int productId) {
    CartItem removed;
    if (!cart.remove(productId, removed))
        return "Item is not in the cart.";

    Product* p = products.search(productId);
    if (p != nullptr) {
        p->quantity += removed.quantity;
        rebuildTree();
    }

    Product info = {removed.productId, removed.name, removed.price, 0};
    undoStack.push({"REMOVE_FROM_CART", info, removed.quantity});
    return "Removed " + removed.name + " from the cart.";
}

// a new sale starts with an empty cart and a fresh undo history
void Supermarket::startSale() {
    cart.clear();
    undoStack.clear();
}

string Supermarket::cancelSale() {
    vector<CartItem> items = cart.traverse();
    for (int i = 0; i < (int)items.size(); i++) {
        Product* p = products.search(items[i].productId);
        if (p != nullptr)
            p->quantity += items[i].quantity;
    }
    cart.clear();
    undoStack.clear();
    rebuildTree();
    return "Sale cancelled. Items returned to the shelves.";
}

string Supermarket::checkout() {
    if (cart.isEmpty())
        return "The cart is empty.";

    double total = cart.total();
    totalSales += total;
    itemsSold += cart.itemCount();
    cart.clear();
    undoStack.clear();
    return "Checkout complete. Total paid: " + money(total);
}

vector<CartItem> Supermarket::getCart() {
    return cart.traverse();
}

double Supermarket::cartTotal() {
    return cart.total();
}

// ---------------- Restocking ----------------

string Supermarket::requestRestock(int productId, int amount) {
    Product* p = products.search(productId);
    if (p == nullptr)
        return "Product not found.";
    if (amount <= 0)
        return "Amount must be more than zero.";
    if (restockQueue.isFull())
        return "Too many pending orders.";

    restockQueue.enqueue({productId, amount});
    return "Order placed: " + to_string(amount) + " x " + p->name + ".";
}

string Supermarket::processRestock() {
    RestockRequest r;
    if (!restockQueue.dequeue(r))
        return "No pending deliveries.";

    Product* p = products.search(r.productId);
    if (p == nullptr)
        return "Delivery skipped, product " + to_string(r.productId) + " no longer exists.";

    p->quantity += r.amount;
    rebuildTree();
    return "Delivery received: " + to_string(r.amount) + " x " + p->name + ". Now in stock: " + to_string(p->quantity);
}

vector<RestockRequest> Supermarket::getRestockQueue() {
    return restockQueue.getAll();
}

string Supermarket::restockStatus() {
    if (restockQueue.isEmpty())
        return "Restock queue is empty.";

    string status = to_string(restockQueue.size()) + "/" + to_string(restockQueue.getCapacity()) + " waiting";
    if (restockQueue.isFull())
        status += " | FULL";
    return status;
}

// ---------------- Undo ----------------

string Supermarket::undo() {
    Operation op;
    if (!undoStack.pop(op))
        return "Nothing to undo.";

    Product p = op.product;

    if (op.type == "ADD_PRODUCT") {
        products.remove(p.id);
    } else if (op.type == "DELETE_PRODUCT") {
        products.insert(p);
    } else if (op.type == "UPDATE_PRODUCT") {
        products.updatePrice(p.id, p.price);
        products.updateQuantity(p.id, p.quantity);
    } else if (op.type == "ADD_TO_CART") {
        cart.removeQuantity(p.id, op.quantity);
        Product* stock = products.search(p.id);
        if (stock != nullptr)
            stock->quantity += op.quantity;
    } else if (op.type == "REMOVE_FROM_CART") {
        cart.add({p.id, p.name, p.price, op.quantity});
        Product* stock = products.search(p.id);
        if (stock != nullptr)
            stock->quantity -= op.quantity;
    }

    rebuildTree();
    return "Undone: " + describe(op);
}

string Supermarket::lastOperation() {
    if (undoStack.isEmpty())
        return "Nothing to undo.";
    return describe(undoStack.peek());
}

string Supermarket::describe(Operation op) {
    string name = op.product.name;
    if (op.type == "ADD_PRODUCT") return "Added product " + name;
    if (op.type == "DELETE_PRODUCT") return "Removed product " + name;
    if (op.type == "UPDATE_PRODUCT") return "Changed " + name;
    if (op.type == "ADD_TO_CART") return "Scanned " + to_string(op.quantity) + " x " + name;
    if (op.type == "REMOVE_FROM_CART") return "Removed " + name + " from the cart";
    return op.type;
}

// ---------------- Report ----------------

double Supermarket::getTotalSales() {
    return totalSales;
}

int Supermarket::getItemsSold() {
    return itemsSold;
}

int Supermarket::getCustomersServed() {
    return customersServed;
}

vector<Product> Supermarket::getLowStock(int limit) {
    vector<Product> all = sortedProducts.inorder();
    vector<Product> result;
    for (int i = 0; i < (int)all.size(); i++) {
        if (all[i].quantity <= limit)
            result.push_back(all[i]);
    }
    return result;
}

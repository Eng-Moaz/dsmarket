#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include "Supermarket.h"
using namespace std;

const int LOW_STOCK = 5;

Supermarket market;

// ---------------- Input / output helpers ----------------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int readInt(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof())
            exit(0);
        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(string prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof())
            exit(0);
        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readText(string prompt) {
    string text;
    while (text.empty()) {
        cout << prompt;
        if (!getline(cin, text))
            exit(0);
    }
    return text;
}

bool askYesNo(string question) {
    string answer = readText(question + " (y/n): ");
    return answer == "y" || answer == "Y";
}

void pause() {
    cout << "\nPress Enter to continue...";
    string line;
    if (!getline(cin, line))
        exit(0);
}

void header(string title) {
    clearScreen();
    cout << "==================================================\n";
    cout << "  " << title << "\n";
    cout << "==================================================\n";
}

void showMessage(string message) {
    if (!message.empty())
        cout << ">> " << message << "\n\n";
}

void printProducts(vector<Product> list) {
    if (list.empty()) {
        cout << "No products.\n";
        return;
    }
    cout << left << setw(8) << "Code" << setw(20) << "Name" << setw(10) << "Price" << "Stock\n";
    cout << string(46, '-') << "\n";
    for (int i = 0; i < (int)list.size(); i++) {
        cout << left << setw(8) << list[i].id << setw(20) << list[i].name
             << setw(10) << fixed << setprecision(2) << list[i].price << list[i].quantity;
        if (list[i].quantity <= LOW_STOCK)
            cout << "  (low)";
        cout << "\n";
    }
}

void printCart() {
    vector<CartItem> items = market.getCart();
    if (items.empty()) {
        cout << "Cart is empty.\n";
        return;
    }
    cout << left << setw(8) << "Code" << setw(20) << "Item" << setw(6) << "Qty" << "Price\n";
    cout << string(42, '-') << "\n";
    for (int i = 0; i < (int)items.size(); i++) {
        cout << left << setw(8) << items[i].productId << setw(20) << items[i].name
             << setw(6) << items[i].quantity
             << fixed << setprecision(2) << items[i].price * items[i].quantity << "\n";
    }
    cout << string(42, '-') << "\n";
    cout << left << setw(34) << "TOTAL" << market.cartTotal() << "\n";
}

int choosePriority() {
    cout << "1. Regular\n2. Express\n3. VIP\n";
    return readInt("Choose: ");
}

string undoLastAction() {
    string last = market.lastOperation();
    if (last == "Nothing to undo.")
        return last;
    if (askYesNo("Undo \"" + last + "\"?"))
        return market.undo();
    return "";
}

// ---------------- Cashier ----------------

void saleScreen(Customer customer) {
    market.startSale();
    string message = "";

    while (true) {
        header("Sale - " + customer.name + " (" + priorityName(customer.priority) + ")");
        printCart();
        cout << "\n";
        showMessage(message);
        message = "";

        cout << "1. Scan item\n"
             << "2. Remove item\n"
             << "3. Undo last action\n"
             << "4. Pay\n"
             << "5. Cancel sale\n";
        int choice = readInt("Choose: ");

        if (choice == 1) {
            int code = readInt("Product code: ");
            int quantity = readInt("Quantity: ");
            message = market.addToCart(code, quantity);
        } else if (choice == 2) {
            message = market.removeFromCart(readInt("Product code: "));
        } else if (choice == 3) {
            message = undoLastAction();
        } else if (choice == 4) {
            double total = market.cartTotal();
            if (total == 0) {
                message = "The cart is empty.";
                continue;
            }
            double paid = readDouble("Amount paid: ");
            if (paid < total) {
                message = "Not enough money.";
                continue;
            }

            header("Receipt - " + customer.name);
            printCart();
            cout << left << setw(34) << "Paid" << paid << "\n";
            cout << left << setw(34) << "Change" << paid - total << "\n\n";
            market.checkout();
            cout << "Thank you for shopping with us!\n";
            pause();
            return;
        } else if (choice == 5) {
            if (askYesNo("Cancel this sale?")) {
                cout << market.cancelSale() << "\n";
                pause();
                return;
            }
        } else {
            message = "Invalid choice.";
        }
    }
}

void cashierMenu() {
    string message = "";

    while (true) {
        header("Cashier");
        vector<Customer> line = market.getCheckoutLane();
        if (line.empty())
            cout << "No customers waiting at checkout.\n\n";
        else
            cout << "Waiting: " << line.size() << "   Next: " << line[0].name
                 << " (" << priorityName(line[0].priority) << ")\n\n";
        showMessage(message);
        message = "";

        cout << "1. Customer joins the checkout line\n"
             << "2. Serve next customer\n"
             << "3. View checkout line\n"
             << "4. Change a customer's priority\n"
             << "0. Back\n";
        int choice = readInt("Choose: ");

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            string name = readText("Customer name: ");
            bool vip = askYesNo("VIP member?");
            int items = readInt("Number of items in basket: ");

            int priority = 1;
            if (vip)
                priority = 3;
            else if (items <= 10)
                priority = 2;

            message = market.addToCheckoutLane(name, priority);
        } else if (choice == 2) {
            if (line.empty()) {
                message = "No customers waiting.";
                continue;
            }
            Customer next = line[0];
            market.serveCheckoutLane();
            saleScreen(next);
        } else if (choice == 3) {
            if (line.empty())
                cout << "Nobody is waiting.\n";
            for (int i = 0; i < (int)line.size(); i++)
                cout << i + 1 << ". " << line[i].name << " (" << priorityName(line[i].priority) << ")\n";
            pause();
        } else if (choice == 4) {
            if (line.empty()) {
                message = "No customers waiting.";
                continue;
            }
            for (int i = 0; i < (int)line.size(); i++)
                cout << i + 1 << ". " << line[i].name << " (" << priorityName(line[i].priority) << ")\n";
            int number = readInt("Customer number: ");
            if (number < 1 || number > (int)line.size()) {
                message = "Invalid customer number.";
                continue;
            }
            message = market.changePriority(line[number - 1].id, choosePriority());
        } else {
            message = "Invalid choice.";
        }
    }
}

// ---------------- Inventory ----------------

void inventoryMenu() {
    string message = "";

    while (true) {
        header("Inventory");
        showMessage(message);
        message = "";

        cout << "1. View all products\n"
             << "2. Find product\n"
             << "3. Add new product\n"
             << "4. Change price\n"
             << "5. Adjust stock\n"
             << "6. Remove product\n"
             << "7. Undo last change\n"
             << "0. Back\n";
        int choice = readInt("Choose: ");

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            printProducts(market.getSortedProducts());
            pause();
        } else if (choice == 2) {
            Product* p = market.findProduct(readInt("Product code: "));
            if (p == nullptr) {
                message = "Product not found.";
                continue;
            }
            cout << "\nCode:  " << p->id << "\nName:  " << p->name
                 << "\nPrice: " << fixed << setprecision(2) << p->price
                 << "\nStock: " << p->quantity << "\n";
            pause();
        } else if (choice == 3) {
            int code = readInt("Product code: ");
            if (market.findProduct(code) != nullptr) {
                message = "This code is already used.";
                continue;
            }
            string name = readText("Name: ");
            double price = readDouble("Price: ");
            int quantity = readInt("Stock: ");
            message = market.addProduct(code, name, price, quantity);
        } else if (choice == 4) {
            Product* p = market.findProduct(readInt("Product code: "));
            if (p == nullptr) {
                message = "Product not found.";
                continue;
            }
            cout << p->name << " costs " << fixed << setprecision(2) << p->price << "\n";
            message = market.updatePrice(p->id, readDouble("New price: "));
        } else if (choice == 5) {
            Product* p = market.findProduct(readInt("Product code: "));
            if (p == nullptr) {
                message = "Product not found.";
                continue;
            }
            cout << p->name << " has " << p->quantity << " in stock\n";
            message = market.updateQuantity(p->id, readInt("New stock: "));
        } else if (choice == 6) {
            Product* p = market.findProduct(readInt("Product code: "));
            if (p == nullptr) {
                message = "Product not found.";
                continue;
            }
            if (askYesNo("Remove " + p->name + "?"))
                message = market.deleteProduct(p->id);
        } else if (choice == 7) {
            message = undoLastAction();
        } else {
            message = "Invalid choice.";
        }
    }
}

// ---------------- Deliveries ----------------

void deliveriesMenu() {
    string message = "";

    while (true) {
        header("Stock Deliveries");

        vector<Product> low = market.getLowStock(LOW_STOCK);
        if (low.empty()) {
            cout << "All products are well stocked.\n";
        } else {
            cout << "Running low:\n";
            for (int i = 0; i < (int)low.size(); i++)
                cout << "  - " << low[i].name << " (code " << low[i].id << ", " << low[i].quantity << " left)\n";
        }
        cout << "Pending orders: " << market.getRestockQueue().size() << "\n\n";
        showMessage(message);
        message = "";

        cout << "1. Order stock\n"
             << "2. Receive next delivery\n"
             << "3. View pending orders\n"
             << "0. Back\n";
        int choice = readInt("Choose: ");

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            int code = readInt("Product code: ");
            int amount = readInt("Amount to order: ");
            message = market.requestRestock(code, amount);
        } else if (choice == 2) {
            message = market.processRestock();
        } else if (choice == 3) {
            vector<RestockRequest> orders = market.getRestockQueue();
            if (orders.empty())
                cout << "No pending orders.\n";
            for (int i = 0; i < (int)orders.size(); i++) {
                Product* p = market.findProduct(orders[i].productId);
                string name = (p != nullptr) ? p->name : "(removed product)";
                cout << i + 1 << ". " << name << " x " << orders[i].amount << "\n";
            }
            pause();
        } else {
            message = "Invalid choice.";
        }
    }
}

// ---------------- Service desk ----------------

void serviceDeskMenu() {
    string message = "";

    while (true) {
        header("Customer Service Desk");
        vector<Customer> waiting = market.getServiceLine();
        if (waiting.empty())
            cout << "Nobody is waiting.\n\n";
        else
            cout << "Waiting: " << waiting.size() << "   Next: " << waiting[0].name << "\n\n";
        showMessage(message);
        message = "";

        cout << "1. New customer (take a ticket)\n"
             << "2. Call next customer\n"
             << "3. View waiting list\n"
             << "0. Back\n";
        int choice = readInt("Choose: ");

        if (choice == 0) {
            return;
        } else if (choice == 1) {
            message = market.addToServiceLine(readText("Customer name: "));
        } else if (choice == 2) {
            message = market.serveServiceLine();
        } else if (choice == 3) {
            if (waiting.empty())
                cout << "Nobody is waiting.\n";
            for (int i = 0; i < (int)waiting.size(); i++)
                cout << i + 1 << ". " << waiting[i].name << " (ticket " << waiting[i].id << ")\n";
            pause();
        } else {
            message = "Invalid choice.";
        }
    }
}

// ---------------- Report ----------------

void dailyReport() {
    header("End of Day Report");
    cout << "Total sales:       " << fixed << setprecision(2) << market.getTotalSales() << "\n";
    cout << "Items sold:        " << market.getItemsSold() << "\n";
    cout << "Customers served:  " << market.getCustomersServed() << "\n";
    cout << "Pending orders:    " << market.getRestockQueue().size() << "\n\n";
    cout << "Products running low:\n";
    printProducts(market.getLowStock(LOW_STOCK));
    pause();
}

// ---------------- Main menu ----------------

int main() {
    market.loadSampleData();

    while (true) {
        header("SUPERMARKET MANAGEMENT SYSTEM");
        cout << "Checkout line: " << market.getCheckoutLane().size()
             << "  |  Service desk: " << market.getServiceLine().size()
             << "  |  Low stock: " << market.getLowStock(LOW_STOCK).size() << "\n\n";

        cout << "1. Cashier\n"
             << "2. Inventory\n"
             << "3. Stock deliveries\n"
             << "4. Customer service desk\n"
             << "5. End of day report\n"
             << "0. Exit\n";
        int choice = readInt("Choose: ");

        if (choice == 0)
            break;
        else if (choice == 1)
            cashierMenu();
        else if (choice == 2)
            inventoryMenu();
        else if (choice == 3)
            deliveriesMenu();
        else if (choice == 4)
            serviceDeskMenu();
        else if (choice == 5)
            dailyReport();
    }

    cout << "Goodbye!\n";
    return 0;
}

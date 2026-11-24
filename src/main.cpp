#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include "Supermarket.h"
using namespace std;

Supermarket market;

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

void pause() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void printTitle(string title) {
    cout << "\n========== " << title << " ==========\n";
}

void printProducts(vector<Product> list) {
    if (list.empty()) {
        cout << "No products.\n";
        return;
    }
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Price" << "Quantity\n";
    cout << string(46, '-') << "\n";
    for (int i = 0; i < (int)list.size(); i++) {
        cout << left << setw(8) << list[i].id << setw(20) << list[i].name
             << setw(10) << fixed << setprecision(2) << list[i].price << list[i].quantity << "\n";
    }
}

void printProduct(Product* p) {
    if (p == nullptr) {
        cout << "Product not found.\n";
        return;
    }
    cout << "ID: " << p->id << "\nName: " << p->name
         << "\nPrice: " << fixed << setprecision(2) << p->price
         << "\nQuantity: " << p->quantity << "\n";
}

void printCustomers(vector<Customer> list, bool showPriority) {
    if (list.empty()) {
        cout << "Nobody is waiting.\n";
        return;
    }
    for (int i = 0; i < (int)list.size(); i++) {
        cout << i + 1 << ". " << list[i].name << " (ID " << list[i].id << ")";
        if (showPriority)
            cout << " [" << priorityName(list[i].priority) << "]";
        cout << "\n";
    }
}

void productsMenu() {
    while (true) {
        printTitle("Products");
        cout << "1. Add product\n"
             << "2. Search product (Hash Table)\n"
             << "3. Search product (BST)\n"
             << "4. Update price\n"
             << "5. Update quantity\n"
             << "6. Delete product\n"
             << "7. Show products sorted by ID\n"
             << "0. Back\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            return;

        if (choice == 1) {
            int id = readInt("ID: ");
            string name = readText("Name: ");
            double price = readDouble("Price: ");
            int quantity = readInt("Quantity: ");
            cout << market.addProduct(id, name, price, quantity) << "\n";
        } else if (choice == 2) {
            printProduct(market.findProduct(readInt("ID: ")));
        } else if (choice == 3) {
            printProduct(market.findProductInTree(readInt("ID: ")));
        } else if (choice == 4) {
            int id = readInt("ID: ");
            double price = readDouble("New price: ");
            cout << market.updatePrice(id, price) << "\n";
        } else if (choice == 5) {
            int id = readInt("ID: ");
            int quantity = readInt("New quantity: ");
            cout << market.updateQuantity(id, quantity) << "\n";
        } else if (choice == 6) {
            cout << market.deleteProduct(readInt("ID: ")) << "\n";
        } else if (choice == 7) {
            printProducts(market.getSortedProducts());
        } else {
            cout << "Invalid choice.\n";
        }
        pause();
    }
}

void serviceLineMenu() {
    while (true) {
        printTitle("Customer Service Line");
        cout << market.serviceLineStatus() << "\n\n";
        cout << "1. Add customer\n"
             << "2. Serve next customer\n"
             << "3. Show line\n"
             << "0. Back\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            return;

        if (choice == 1)
            cout << market.addToServiceLine(readText("Customer name: ")) << "\n";
        else if (choice == 2)
            cout << market.serveServiceLine() << "\n";
        else if (choice == 3)
            printCustomers(market.getServiceLine(), false);
        else
            cout << "Invalid choice.\n";
        pause();
    }
}

void checkoutLaneMenu() {
    while (true) {
        printTitle("Express & VIP Checkout Lane");
        cout << market.peekCheckoutLane() << "\n\n";
        cout << "1. Add customer\n"
             << "2. Serve next customer\n"
             << "3. Change customer priority\n"
             << "4. Show lane (serving order)\n"
             << "0. Back\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            return;

        if (choice == 1) {
            string name = readText("Customer name: ");
            int priority = readInt("Priority (1 = Normal, 2 = Express, 3 = VIP): ");
            cout << market.addToCheckoutLane(name, priority) << "\n";
        } else if (choice == 2) {
            cout << market.serveCheckoutLane() << "\n";
        } else if (choice == 3) {
            int id = readInt("Customer ID: ");
            int priority = readInt("New priority (1 = Normal, 2 = Express, 3 = VIP): ");
            cout << market.changePriority(id, priority) << "\n";
        } else if (choice == 4) {
            printCustomers(market.getCheckoutLane(), true);
        } else {
            cout << "Invalid choice.\n";
        }
        pause();
    }
}

void cartMenu() {
    while (true) {
        printTitle("Shopping Cart");
        cout << "1. Add item\n"
             << "2. Remove item\n"
             << "3. Show cart\n"
             << "4. Checkout\n"
             << "0. Back\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            return;

        if (choice == 1) {
            int id = readInt("Product ID: ");
            int quantity = readInt("Quantity: ");
            cout << market.addToCart(id, quantity) << "\n";
        } else if (choice == 2) {
            cout << market.removeFromCart(readInt("Product ID: ")) << "\n";
        } else if (choice == 3) {
            vector<CartItem> items = market.getCart();
            if (items.empty()) {
                cout << "The cart is empty.\n";
            } else {
                cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(6) << "Qty" << "Subtotal\n";
                cout << string(42, '-') << "\n";
                for (int i = 0; i < (int)items.size(); i++) {
                    cout << left << setw(8) << items[i].productId << setw(20) << items[i].name
                         << setw(6) << items[i].quantity
                         << fixed << setprecision(2) << items[i].price * items[i].quantity << "\n";
                }
                cout << "Total: " << market.cartTotal() << "\n";
            }
        } else if (choice == 4) {
            cout << market.checkout() << "\n";
        } else {
            cout << "Invalid choice.\n";
        }
        pause();
    }
}

void restockMenu() {
    while (true) {
        printTitle("Restocking");
        cout << market.restockStatus() << "\n\n";
        cout << "1. Request restock\n"
             << "2. Process next restock\n"
             << "3. Show restock queue\n"
             << "0. Back\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            return;

        if (choice == 1) {
            int id = readInt("Product ID: ");
            int amount = readInt("Amount: ");
            cout << market.requestRestock(id, amount) << "\n";
        } else if (choice == 2) {
            cout << market.processRestock() << "\n";
        } else if (choice == 3) {
            vector<RestockRequest> list = market.getRestockQueue();
            if (list.empty())
                cout << "Nothing waiting.\n";
            for (int i = 0; i < (int)list.size(); i++) {
                Product* p = market.findProduct(list[i].productId);
                string name = (p != nullptr) ? p->name : "(deleted)";
                cout << i + 1 << ". " << name << " +" << list[i].amount << "\n";
            }
        } else {
            cout << "Invalid choice.\n";
        }
        pause();
    }
}

void undoMenu() {
    printTitle("Undo");
    cout << "Last operation: " << market.lastOperation() << "\n";
    cout << "1. Undo it\n0. Back\n";
    if (readInt("Choice: ") == 1)
        cout << market.undo() << "\n";
    pause();
}

void dailyReport() {
    printTitle("Daily Report");
    cout << "Total sales: " << fixed << setprecision(2) << market.getTotalSales() << "\n";
    cout << "Items sold: " << market.getItemsSold() << "\n";
    cout << "Customers served: " << market.getCustomersServed() << "\n";
    cout << "\nLow stock (5 or less):\n";
    printProducts(market.getLowStock(5));
    pause();
}

int main() {
    market.loadSampleData();

    while (true) {
        printTitle("Supermarket Management System");
        cout << "1. Products\n"
             << "2. Customer service line\n"
             << "3. Express & VIP checkout lane\n"
             << "4. Shopping cart\n"
             << "5. Restocking\n"
             << "6. Undo last operation\n"
             << "7. Daily report\n"
             << "0. Exit\n";
        int choice = readInt("Choice: ");

        if (choice == 0)
            break;

        if (choice == 1) productsMenu();
        else if (choice == 2) serviceLineMenu();
        else if (choice == 3) checkoutLaneMenu();
        else if (choice == 4) cartMenu();
        else if (choice == 5) restockMenu();
        else if (choice == 6) undoMenu();
        else if (choice == 7) dailyReport();
        else cout << "Invalid choice.\n";
    }

    cout << "Goodbye!\n";
    return 0;
}

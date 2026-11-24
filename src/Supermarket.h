#pragma once
#include <string>
#include <vector>
#include "Product.h"
#include "Customer.h"
#include "HashTable.h"
#include "Queue.h"
#include "PriorityQueue.h"
#include "Stack.h"
#include "LinkedList.h"
#include "BST.h"
using namespace std;

struct RestockRequest {
    int productId;
    int amount;
};

class Supermarket {
private:
    HashTable products;
    BST sortedProducts;
    Queue<Customer> serviceLine;
    PriorityQueue checkoutLane;
    Stack undoStack;
    LinkedList cart;
    Queue<RestockRequest> restockQueue;

    int nextCustomerId;
    int clock;
    double totalSales;
    int itemsSold;
    int customersServed;

    void rebuildTree();
    string describe(Operation op);

public:
    Supermarket();
    void loadSampleData();

    // Products (Hash Table + BST)
    string addProduct(int id, string name, double price, int quantity);
    string deleteProduct(int id);
    string updatePrice(int id, double newPrice);
    string updateQuantity(int id, int newQuantity);
    Product* findProduct(int id);
    Product* findProductInTree(int id);
    vector<Product> getAllProducts();
    vector<Product> getSortedProducts();

    // Customer service line (Queue)
    string addToServiceLine(string name);
    string serveServiceLine();
    vector<Customer> getServiceLine();
    string serviceLineStatus();

    // Express / VIP checkout lane (Priority Queue)
    string addToCheckoutLane(string name, int priority);
    string serveCheckoutLane();
    string peekCheckoutLane();
    string changePriority(int customerId, int newPriority);
    vector<Customer> getCheckoutLane();

    // Shopping cart (Linked List)
    string addToCart(int productId, int quantity);
    string removeFromCart(int productId);
    void startSale();
    string checkout();
    string cancelSale();
    vector<CartItem> getCart();
    double cartTotal();

    // Restocking (Queue)
    string requestRestock(int productId, int amount);
    string processRestock();
    vector<RestockRequest> getRestockQueue();
    string restockStatus();

    // Undo (Stack)
    string undo();
    string lastOperation();

    // Daily report
    double getTotalSales();
    int getItemsSold();
    int getCustomersServed();
    vector<Product> getLowStock(int limit);
};

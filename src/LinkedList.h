#pragma once
#include <string>
#include <vector>
using namespace std;

struct CartItem {
    int productId;
    string name;
    double price;
    int quantity;
};

// Singly linked list used as the shopping cart
class LinkedList {
private:
    struct Node {
        CartItem data;
        Node* next;
    };

    Node* head;

public:
    LinkedList() {
        head = nullptr;
    }

    ~LinkedList() {
        clear();
    }

    bool isEmpty() {
        return head == nullptr;
    }

    CartItem* find(int productId) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.productId == productId)
                return &current->data;
            current = current->next;
        }
        return nullptr;
    }

    // if the product is already in the cart we just increase its quantity
    void add(CartItem item) {
        CartItem* existing = find(item.productId);
        if (existing != nullptr) {
            existing->quantity += item.quantity;
            return;
        }

        Node* newNode = new Node;
        newNode->data = item;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
            return;
        }

        Node* current = head;
        while (current->next != nullptr)
            current = current->next;
        current->next = newNode;
    }

    bool remove(int productId, CartItem& removed) {
        Node* current = head;
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->data.productId == productId) {
                if (prev == nullptr)
                    head = current->next;
                else
                    prev->next = current->next;
                removed = current->data;
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    // decreases the quantity, and removes the item if it reaches zero
    bool removeQuantity(int productId, int amount) {
        CartItem* item = find(productId);
        if (item == nullptr)
            return false;

        item->quantity -= amount;
        if (item->quantity <= 0) {
            CartItem removed;
            remove(productId, removed);
        }
        return true;
    }

    vector<CartItem> traverse() {
        vector<CartItem> result;
        Node* current = head;
        while (current != nullptr) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }

    double total() {
        double sum = 0;
        Node* current = head;
        while (current != nullptr) {
            sum += current->data.price * current->data.quantity;
            current = current->next;
        }
        return sum;
    }

    int itemCount() {
        int sum = 0;
        Node* current = head;
        while (current != nullptr) {
            sum += current->data.quantity;
            current = current->next;
        }
        return sum;
    }

    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

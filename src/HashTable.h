#pragma once
#include <vector>
#include "Product.h"
using namespace std;

class HashTable {
private:
    struct Node {
        Product data;
        Node* next;
    };

    static const int SIZE = 31;
    Node* table[SIZE];
    int count;

    int hash(int id) {
        return id % SIZE;
    }

public:
    HashTable() {
        count = 0;
        for (int i = 0; i < SIZE; i++)
            table[i] = nullptr;
    }

    ~HashTable() {
        for (int i = 0; i < SIZE; i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }

    bool insert(Product p) {
        if (search(p.id) != nullptr)
            return false;

        int index = hash(p.id);
        Node* newNode = new Node;
        newNode->data = p;
        newNode->next = table[index];
        table[index] = newNode;
        count++;
        return true;
    }

    Product* search(int id) {
        Node* current = table[hash(id)];
        while (current != nullptr) {
            if (current->data.id == id)
                return &current->data;
            current = current->next;
        }
        return nullptr;
    }

    bool remove(int id) {
        int index = hash(id);
        Node* current = table[index];
        Node* prev = nullptr;

        while (current != nullptr) {
            if (current->data.id == id) {
                if (prev == nullptr)
                    table[index] = current->next;
                else
                    prev->next = current->next;
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    bool updatePrice(int id, double newPrice) {
        Product* p = search(id);
        if (p == nullptr)
            return false;
        p->price = newPrice;
        return true;
    }

    bool updateQuantity(int id, int newQuantity) {
        Product* p = search(id);
        if (p == nullptr)
            return false;
        p->quantity = newQuantity;
        return true;
    }

    vector<Product> getAll() {
        vector<Product> result;
        for (int i = 0; i < SIZE; i++) {
            Node* current = table[i];
            while (current != nullptr) {
                result.push_back(current->data);
                current = current->next;
            }
        }
        return result;
    }

    int size() {
        return count;
    }
};

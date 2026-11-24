#pragma once
#include <string>
#include "Product.h"
using namespace std;

struct Operation {
    string type;
    Product product;
    int quantity;
};

// Stack using a linked list
class Stack {
private:
    struct Node {
        Operation data;
        Node* next;
    };

    Node* top;
    int count;

public:
    Stack() {
        top = nullptr;
        count = 0;
    }

    ~Stack() {
        clear();
    }

    bool isEmpty() {
        return top == nullptr;
    }

    int size() {
        return count;
    }

    void push(Operation op) {
        Node* newNode = new Node;
        newNode->data = op;
        newNode->next = top;
        top = newNode;
        count++;
    }

    bool pop(Operation& op) {
        if (isEmpty())
            return false;
        Node* temp = top;
        op = temp->data;
        top = top->next;
        delete temp;
        count--;
        return true;
    }

    // call isEmpty() before peek()
    Operation peek() {
        return top->data;
    }

    void clear() {
        Operation op;
        while (pop(op)) {
        }
    }
};

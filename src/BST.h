#pragma once
#include <vector>
#include "Product.h"
using namespace std;

// Binary search tree of products ordered by product ID
class BST {
private:
    struct Node {
        Product data;
        Node* left;
        Node* right;
    };

    Node* root;

    Node* insert(Node* node, Product p) {
        if (node == nullptr) {
            Node* newNode = new Node;
            newNode->data = p;
            newNode->left = nullptr;
            newNode->right = nullptr;
            return newNode;
        }

        if (p.id < node->data.id)
            node->left = insert(node->left, p);
        else if (p.id > node->data.id)
            node->right = insert(node->right, p);

        return node;
    }

    void inorder(Node* node, vector<Product>& result) {
        if (node == nullptr)
            return;
        inorder(node->left, result);
        result.push_back(node->data);
        inorder(node->right, result);
    }

    void destroy(Node* node) {
        if (node == nullptr)
            return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BST() {
        root = nullptr;
    }

    ~BST() {
        destroy(root);
    }

    void insert(Product p) {
        root = insert(root, p);
    }

    Product* search(int id) {
        Node* current = root;
        while (current != nullptr) {
            if (id == current->data.id)
                return &current->data;
            if (id < current->data.id)
                current = current->left;
            else
                current = current->right;
        }
        return nullptr;
    }

    vector<Product> inorder() {
        vector<Product> result;
        inorder(root, result);
        return result;
    }

    void clear() {
        destroy(root);
        root = nullptr;
    }
};

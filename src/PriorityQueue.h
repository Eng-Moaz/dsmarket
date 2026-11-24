#pragma once
#include <vector>
#include "Customer.h"
using namespace std;

// Max-heap: higher priority first, if equal then earlier arrival first
class PriorityQueue {
private:
    Customer* heap;
    int capacity;
    int count;

    bool comesFirst(Customer a, Customer b) {
        if (a.priority != b.priority)
            return a.priority > b.priority;
        return a.arrivalTime < b.arrivalTime;
    }

    void swapItems(int i, int j) {
        Customer temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (comesFirst(heap[i], heap[parent])) {
                swapItems(i, parent);
                i = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int best = i;

            if (left < count && comesFirst(heap[left], heap[best]))
                best = left;
            if (right < count && comesFirst(heap[right], heap[best]))
                best = right;

            if (best == i)
                break;

            swapItems(i, best);
            i = best;
        }
    }

public:
    PriorityQueue(int cap = 50) {
        capacity = cap;
        heap = new Customer[capacity];
        count = 0;
    }

    ~PriorityQueue() {
        delete[] heap;
    }

    bool isEmpty() {
        return count == 0;
    }

    bool isFull() {
        return count == capacity;
    }

    int size() {
        return count;
    }

    bool insert(Customer c) {
        if (isFull())
            return false;
        heap[count] = c;
        heapifyUp(count);
        count++;
        return true;
    }

    bool extractTop(Customer& c) {
        if (isEmpty())
            return false;
        c = heap[0];
        heap[0] = heap[count - 1];
        count--;
        heapifyDown(0);
        return true;
    }

    // call isEmpty() before peek()
    Customer peek() {
        return heap[0];
    }

    bool updatePriority(int customerId, int newPriority) {
        for (int i = 0; i < count; i++) {
            if (heap[i].id == customerId) {
                heap[i].priority = newPriority;
                heapifyUp(i);
                heapifyDown(i);
                return true;
            }
        }
        return false;
    }

    // returns customers in the order they will be served
    vector<Customer> getAll() {
        PriorityQueue copy(capacity);
        for (int i = 0; i < count; i++)
            copy.insert(heap[i]);

        vector<Customer> result;
        Customer c;
        while (copy.extractTop(c))
            result.push_back(c);
        return result;
    }
};

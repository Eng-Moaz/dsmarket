#pragma once
#include <vector>
using namespace std;

// Circular array queue
template <typename T>
class Queue {
private:
    T* items;
    int capacity;
    int frontIndex;
    int rearIndex;
    int count;

public:
    Queue(int cap = 20) {
        capacity = cap;
        items = new T[capacity];
        frontIndex = 0;
        rearIndex = -1;
        count = 0;
    }

    ~Queue() {
        delete[] items;
    }

    bool isEmpty() {
        return count == 0;
    }

    bool isFull() {
        return count == capacity;
    }

    bool enqueue(T item) {
        if (isFull())
            return false;
        rearIndex = (rearIndex + 1) % capacity;
        items[rearIndex] = item;
        count++;
        return true;
    }

    bool dequeue(T& item) {
        if (isEmpty())
            return false;
        item = items[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        count--;
        return true;
    }

    // call isEmpty() before front() and back()
    T front() {
        return items[frontIndex];
    }

    T back() {
        return items[rearIndex];
    }

    int size() {
        return count;
    }

    int getCapacity() {
        return capacity;
    }

    vector<T> getAll() {
        vector<T> result;
        for (int i = 0; i < count; i++)
            result.push_back(items[(frontIndex + i) % capacity]);
        return result;
    }
};

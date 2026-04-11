#ifndef MINHEAP_H
#define MINHEAP_H

#include <iostream>
#include "DynamicArray.h"

// generic min-heap on any type T
template <typename T>
class MinHeap {
private:
    DynamicArray<T> data;

    // index helpers
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    // swap two elements
    void swap(int i, int j) {
        T temp = data.get(i);
        data.set(i, data.get(j));
        data.set(j, temp);
    }

    // bubble up after insert
    void heapifyUp(int i) {
        while (i > 0 && data.get(i).priority < data.get(parent(i)).priority) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    // push down after extract
    void heapifyDown(int i) {
        int smallest = i;
        int left = leftChild(i);
        int right = rightChild(i);
        int sz = data.size();

        if (left < sz && data.get(left).priority < data.get(smallest).priority)
            smallest = left;
        if (right < sz && data.get(right).priority < data.get(smallest).priority)
            smallest = right;

        if (smallest != i) {
            swap(i, smallest);
            heapifyDown(smallest);
        }
    }

public:
    // insert new element
    void insert(const T& item) {
        data.pushBack(item);
        heapifyUp(data.size() - 1);
    }

    // get smallest without removing
    T peek() {
        return data.get(0);
    }

    // remove and return smallest
    T extractMin() {
        T min = data.get(0);
        data.set(0, data.get(data.size() - 1));
        data.popBack();
        if (data.size() > 0)
            heapifyDown(0);
        return min;
    }

    // check if empty
    bool isEmpty() {
        return data.size() == 0;
    }

    // current size
    int getSize() {
        return data.size();
    }

    // print heap (debug)
    void print() {
        for (int i = 0; i < data.size(); i++)
            std::cout << data.get(i).priority << " ";
        std::cout << std::endl;
    }
};

#endif
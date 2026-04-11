#ifndef MINHEAP_H
#define MINHEAP_H

#include <iostream>
#include "DynamicArray.h"

// generic min-heap on any type T
// T must support: T.priority (double) for comparison
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
        T temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    // bubble up after insert
    void heapifyUp(int i) {
        while (i > 0 && data[i].priority < data[parent(i)].priority) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    // push down after extract
    void heapifyDown(int i) {
        int smallest = i;
        int left = leftChild(i);
        int right = rightChild(i);
        int size = data.getSize();

        if (left < size && data[left].priority < data[smallest].priority)
            smallest = left;
        if (right < size && data[right].priority < data[smallest].priority)
            smallest = right;

        if (smallest != i) {
            swap(i, smallest);
            heapifyDown(smallest);
        }
    }

public:
    // insert new element
    void insert(const T& item) {
        data.push_back(item);
        heapifyUp(data.getSize() - 1);
    }

    // get smallest without removing
    T& peek() {
        return data[0];
    }

    // remove and return smallest
    T extractMin() {
        T min = data[0];
        data[0] = data[data.getSize() - 1];
        data.pop_back();
        if (data.getSize() > 0)
            heapifyDown(0);
        return min;
    }

    // check if empty
    bool isEmpty() {
        return data.getSize() == 0;
    }

    // current size
    int getSize() {
        return data.getSize();
    }

    // print heap (debug only)
    void print() {
        for (int i = 0; i < data.getSize(); i++)
            std::cout << data[i].priority << " ";
        std::cout << std::endl;
    }
};

#endif
#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <iostream>

template <typename T>
class DynamicArray {

private:
    T* data;
    int currentSize;
    int currentCapacity;

    void resize() {
        int newCapacity = currentCapacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < currentSize; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        currentCapacity = newCapacity;
    }

public:
    DynamicArray() {
        currentCapacity = 4;
        currentSize = 0;
        data = new T[currentCapacity];
    }

    ~DynamicArray() {
        delete[] data;
    }

    void pushBack(T newItem) {
        if (currentSize == currentCapacity) {
            resize();
        }
        data[currentSize] = newItem;
        currentSize++;
    }

    void popBack() {
        if (currentSize == 0) {
            std::cout << "Array is empty. Nothing to remove." << std::endl;
            return;
        }
        currentSize--;
    }

    T get(int index) {
        if (index < 0 || index >= currentSize) {
            std::cout << "Invalid index: " << index << std::endl;
            return data[0];
        }
        return data[index];
    }

    void set(int index, T newItem) {
        if (index < 0 || index >= currentSize) {
            std::cout << "Invalid index: " << index << std::endl;
            return;
        }
        data[index] = newItem;
    }

    int size() {
        return currentSize;
    }

    bool isEmpty() {
        if (currentSize == 0) {
            return true;
        } else {
            return false;
        }
    }

    void print() {
        std::cout << "Array contents: ";
        for (int i = 0; i < currentSize; i++) {
            std::cout << data[i];
            if (i < currentSize - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }

    void clear() {
        currentSize = 0;
    }
};

#endif
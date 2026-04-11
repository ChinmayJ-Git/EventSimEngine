#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>

// single node
template <typename T>
struct ListNode {
    T data;
    ListNode* next;

    ListNode(const T& val) {
        data = val;
        next = nullptr;
    }
};

// singly linked list
template <typename T>
class LinkedList {
private:
    ListNode<T>* head;
    ListNode<T>* tail;
    int size;

public:
    // init empty list
    LinkedList() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    // clean up all nodes
    ~LinkedList() {
        clear();
    }

    // add to back
    void pushBack(const T& val) {
        ListNode<T>* newNode = new ListNode<T>(val);
        if (tail == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    // add to front
    void pushFront(const T& val) {
        ListNode<T>* newNode = new ListNode<T>(val);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        size++;
    }

    // remove from front
    T popFront() {
        T val = head->data;
        ListNode<T>* temp = head;
        head = head->next;
        if (head == nullptr)
            tail = nullptr;
        delete temp;
        size--;
        return val;
    }

    // peek front value
    T& front() {
        return head->data;
    }

    // peek back value
    T& back() {
        return tail->data;
    }

    // remove node by value
    void remove(const T& val) {
        ListNode<T>* curr = head;
        ListNode<T>* prev = nullptr;

        while (curr != nullptr) {
            if (curr->data == val) {
                if (prev == nullptr)
                    head = curr->next;
                else
                    prev->next = curr->next;
                if (curr == tail)
                    tail = prev;
                delete curr;
                size--;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    // check if value exists
    bool contains(const T& val) {
        ListNode<T>* curr = head;
        while (curr != nullptr) {
            if (curr->data == val)
                return true;
            curr = curr->next;
        }
        return false;
    }

    // current size
    int getSize() {
        return size;
    }

    // check if empty
    bool isEmpty() {
        return size == 0;
    }

    // delete all nodes
    void clear() {
        while (head != nullptr) {
            ListNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }

    // print all values (debug)
    void print() {
        ListNode<T>* curr = head;
        while (curr != nullptr) {
            std::cout << curr->data << " -> ";
            curr = curr->next;
        }
        std::cout << "NULL" << std::endl;
    }

    // get head pointer (read only)
    ListNode<T>* getHead() {
        return head;
    }
};

#endif
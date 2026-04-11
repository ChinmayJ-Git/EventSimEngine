#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <iostream>

// fixed bucket count
#define MAX_BUCKETS 64

// key-value pair node
template <typename V>
struct HashNode {
    std::string key;
    V value;
    HashNode* next;

    HashNode(const std::string& k, const V& v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

// hash map via separate chaining
template <typename V>
class HashMap {
private:
    HashNode<V>* buckets[MAX_BUCKETS];
    int size;

    // hash string key to index
    int hashFunction(const std::string& key) {
        int hash = 0;
        for (int i = 0; i < (int)key.length(); i++)
            hash = (hash * 31 + key[i]) % MAX_BUCKETS;
        return hash;
    }

public:
    // init empty buckets
    HashMap() {
        for (int i = 0; i < MAX_BUCKETS; i++)
            buckets[i] = nullptr;
        size = 0;
    }

    // clean up all nodes
    ~HashMap() {
        for (int i = 0; i < MAX_BUCKETS; i++) {
            HashNode<V>* curr = buckets[i];
            while (curr != nullptr) {
                HashNode<V>* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    // insert or update key
    void put(const std::string& key, const V& value) {
        int index = hashFunction(key);
        HashNode<V>* curr = buckets[index];

        // update if key exists
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        // insert at front
        HashNode<V>* newNode = new HashNode<V>(key, value);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        size++;
    }

    // get value by key
    V get(const std::string& key) {
        int index = hashFunction(key);
        HashNode<V>* curr = buckets[index];

        while (curr != nullptr) {
            if (curr->key == key)
                return curr->value;
            curr = curr->next;
        }

        // key not found
        return V();
    }

    // check key exists
    bool contains(const std::string& key) {
        int index = hashFunction(key);
        HashNode<V>* curr = buckets[index];

        while (curr != nullptr) {
            if (curr->key == key)
                return true;
            curr = curr->next;
        }
        return false;
    }

    // remove by key
    void remove(const std::string& key) {
        int index = hashFunction(key);
        HashNode<V>* curr = buckets[index];
        HashNode<V>* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr)
                    buckets[index] = curr->next;
                else
                    prev->next = curr->next;
                delete curr;
                size--;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    // current entry count
    int getSize() {
        return size;
    }

    // check if empty
    bool isEmpty() {
        return size == 0;
    }

    // print all entries (debug)
    void print() {
        for (int i = 0; i < MAX_BUCKETS; i++) {
            HashNode<V>* curr = buckets[i];
            while (curr != nullptr) {
                std::cout << curr->key << " : " << curr->value << std::endl;
                curr = curr->next;
            }
        }
    }
};

#endif
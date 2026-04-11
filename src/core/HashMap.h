#ifndef HASHMAP_H
#define HASHMAP_H

#include "LinkedList.h"
#include "../engine/Entity.h"

struct HashEntry
{
    int key;
    Entity *value;

    HashEntry()
    {
        key = 0;
        value = nullptr;
    }

    HashEntry(int newKey, Entity *newValue)
    {
        key = newKey;
        value = newValue;
    }

    bool operator==(const HashEntry &other) const
    {
        return key == other.key;
    }
};

class HashMap
{
private:
    static const int TABLE_SIZE = 101;
    LinkedList<HashEntry> buckets[TABLE_SIZE];
    int count;

    int hash(int key) const
    {
        int index = key % TABLE_SIZE;
        if (index < 0)
        {
            index = index + TABLE_SIZE;
        }
        return index;
    }

public:
    HashMap()
    {
        count = 0;
    }

    void insert(int key, Entity *value)
    {
        int index = hash(key);
        ListNode<HashEntry> *currentNode = buckets[index].getHead();

        while (currentNode != nullptr)
        {
            if (currentNode->data.key == key)
            {
                currentNode->data.value = value;
                return;
            }
            currentNode = currentNode->next;
        }

        HashEntry newEntry(key, value);
        buckets[index].pushBack(newEntry);
        count++;
    }

    Entity *get(int key) const
    {
        int index = hash(key);
        ListNode<HashEntry> *currentNode = buckets[index].getHead();

        while (currentNode != nullptr)
        {
            if (currentNode->data.key == key)
            {
                return currentNode->data.value;
            }
            currentNode = currentNode->next;
        }

        return nullptr;
    }

    void remove(int key)
    {
        int index = hash(key);
        HashEntry target(key, nullptr);

        if (buckets[index].contains(target))
        {
            buckets[index].remove(target);
            count--;
        }
    }

    bool contains(int key) const
    {
        return get(key) != nullptr;
    }

    int getCount() const
    {
        return count;
    }
};

#endif
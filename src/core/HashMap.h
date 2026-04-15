#ifndef HASHMAP_H
#define HASHMAP_H

#include "../engine/Entity.h"

class HashMap
{
private:
    struct Entry
    {
        int key;
        Patient *patient;
        Entry *next;
        Entry(int k, Patient *p) : key(k), patient(p), next(0) {}
    };

    Entry *buckets[101];

    int hash(int key) const
    {
        int index = key % 101;
        if (index < 0)
        {
            index += 101;
        }
        return index;
    }

public:
    HashMap()
    {
        for (int i = 0; i < 101; i++)
        {
            buckets[i] = 0;
        }
    }

    ~HashMap()
    {
        for (int i = 0; i < 101; i++)
        {
            Entry *cur = buckets[i];
            while (cur != 0)
            {
                Entry *next = cur->next;
                delete cur;
                cur = next;
            }
            buckets[i] = 0;
        }
    }

    void insert(int key, Patient *patient)
    {
        int index = hash(key);
        Entry *cur = buckets[index];
        while (cur != 0)
        {
            if (cur->key == key)
            {
                cur->patient = patient;
                return;
            }
            cur = cur->next;
        }
        Entry *node = new Entry(key, patient);
        node->next = buckets[index];
        buckets[index] = node;
    }

    Patient *get(int key) const
    {
        Entry *cur = buckets[hash(key)];
        while (cur != 0)
        {
            if (cur->key == key)
            {
                return cur->patient;
            }
            cur = cur->next;
        }
        return 0;
    }

    void remove(int key)
    {
        int index = hash(key);
        Entry *cur = buckets[index];
        Entry *prev = 0;
        while (cur != 0)
        {
            if (cur->key == key)
            {
                if (prev == 0)
                {
                    buckets[index] = cur->next;
                }
                else
                {
                    prev->next = cur->next;
                }
                delete cur;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }

    bool contains(int key) const
    {
        return get(key) != 0;
    }
};

#endif
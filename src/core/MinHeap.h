#ifndef MINHEAP_H
#define MINHEAP_H

#include "../engine/Event.h"

class MinHeap
{
private:
    Event *heap[500];
    int count;

    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;
            if (heap[index]->time >= heap[parent]->time)
            {
                break;
            }
            Event *tmp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = tmp;
            index = parent;
        }
    }

    void heapifyDown(int index)
    {
        while (true)
        {
            int smallest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            if (left < count && heap[left]->time < heap[smallest]->time)
            {
                smallest = left;
            }
            if (right < count && heap[right]->time < heap[smallest]->time)
            {
                smallest = right;
            }
            if (smallest == index)
            {
                break;
            }
            Event *tmp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = tmp;
            index = smallest;
        }
    }

public:
    MinHeap()
    {
        count = 0;
        for (int i = 0; i < 500; i++)
        {
            heap[i] = 0;
        }
    }

    void insert(Event *newEvent)
    {
        if (newEvent == 0 || count >= 500)
        {
            return;
        }
        heap[count] = newEvent;
        heapifyUp(count);
        count++;
    }

    Event *extractMinimum()
    {
        if (count == 0)
        {
            return 0;
        }
        Event *minEvent = heap[0];
        count--;
        heap[0] = heap[count];
        heap[count] = 0;
        if (count > 0)
        {
            heapifyDown(0);
        }
        return minEvent;
    }

    Event *peekMinimum()
    {
        if (count == 0)
        {
            return 0;
        }
        return heap[0];
    }

    bool isEmpty() { return count == 0; }
    int size() { return count; }
};

#endif
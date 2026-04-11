#ifndef MINHEAP_H
#define MINHEAP_H

#include "src/core/DynamicArray.h"
#include "src/engine/Event.h"

class MinHeap
{
private:
    DynamicArray<Event *> heapData;

    void swapNodes(int firstIndex, int secondIndex)
    {
        Event *temp = heapData.get(firstIndex);
        heapData.set(firstIndex, heapData.get(secondIndex));
        heapData.set(secondIndex, temp);
    }

public:
    void insert(Event *newEvent)
    {
        heapData.pushBack(newEvent);
        heapifyUp(heapData.size() - 1);
    }

    Event *extractMinimum()
    {
        if (isEmpty())
        {
            return nullptr;
        }

        Event *minimumEvent = heapData.get(0);
        int lastIndex = heapData.size() - 1;

        if (lastIndex == 0)
        {
            heapData.popBack();
            return minimumEvent;
        }

        heapData.set(0, heapData.get(lastIndex));
        heapData.popBack();
        heapifyDown(0);

        return minimumEvent;
    }

    Event *peekMinimum() const
    {
        if (isEmpty())
        {
            return nullptr;
        }

        return heapData.get(0);
    }

    bool isEmpty() const
    {
        return heapData.size() == 0;
    }

    int getSize() const
    {
        return heapData.size();
    }

private:
    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parentIndex = (index - 1) / 2;

            Event *currentEvent = heapData.get(index);
            Event *parentEvent = heapData.get(parentIndex);

            if (!(*currentEvent < *parentEvent))
            {
                break;
            }

            swapNodes(index, parentIndex);
            index = parentIndex;
        }
    }

    void heapifyDown(int index)
    {
        int size = heapData.size();

        while (true)
        {
            int smallestIndex = index;
            int leftChildIndex = 2 * index + 1;
            int rightChildIndex = 2 * index + 2;

            if (leftChildIndex < size)
            {
                Event *leftChildEvent = heapData.get(leftChildIndex);
                Event *smallestEvent = heapData.get(smallestIndex);
                if (*leftChildEvent < *smallestEvent)
                {
                    smallestIndex = leftChildIndex;
                }
            }

            if (rightChildIndex < size)
            {
                Event *rightChildEvent = heapData.get(rightChildIndex);
                Event *smallestEvent = heapData.get(smallestIndex);
                if (*rightChildEvent < *smallestEvent)
                {
                    smallestIndex = rightChildIndex;
                }
            }

            if (smallestIndex == index)
            {
                break;
            }

            swapNodes(index, smallestIndex);
            index = smallestIndex;
        }
    }
};

#endif
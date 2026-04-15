#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

template <typename T>
class DynamicArray
{
private:
    T *data;
    int count;
    int capacity;

    // grow when full
    void grow()
    {
        capacity = capacity * 2;
        T *newData = new T[capacity];
        for (int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    DynamicArray()
    {
        capacity = 10;
        count = 0;
        data = new T[capacity];
    }

    ~DynamicArray() { delete[] data; }

    void add(T item)
    {
        if (count == capacity)
        {
            grow();
        }
        data[count] = item;
        count++;
    }

    T get(int index) const
    {
        if (index < 0 || index >= count)
        {
            return T();
        }
        return data[index];
    }

    void set(int index, T item)
    {
        if (index < 0 || index >= count)
        {
            return;
        }
        data[index] = item;
    }

    // shift items left
    void remove(int index)
    {
        if (index < 0 || index >= count)
        {
            return;
        }
        for (int i = index; i < count - 1; i++)
        {
            data[i] = data[i + 1];
        }
        count--;
    }

    int size() const { return count; }

    // reset used length
    void clear() { count = 0; }
};

#endif
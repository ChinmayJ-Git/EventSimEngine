#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <typename T>
class LinkedList
{
public:
    struct Node
    {
        T data;
        Node *next;
        Node *prev;
        Node(T item) : data(item), next(0), prev(0) {}
    };

private:
    Node *head;
    Node *tail;
    int count;

public:
    LinkedList() : head(0), tail(0), count(0) {}
    ~LinkedList()
    {
        while (!isEmpty())
        {
            removeFront();
        }
    }

    void addFront(T item)
    {
        Node *node = new Node(item);
        if (head == 0)
        {
            head = node;
            tail = node;
        }
        else
        {
            node->next = head;
            head->prev = node;
            head = node;
        }
        count++;
    }

    void addBack(T item)
    {
        Node *node = new Node(item);
        if (tail == 0)
        {
            head = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        count++;
    }

    void removeFront()
    {
        if (head == 0)
        {
            return;
        }
        Node *node = head;
        head = head->next;
        if (head != 0)
        {
            head->prev = 0;
        }
        else
        {
            tail = 0;
        }
        delete node;
        count--;
    }

    void removeBack()
    {
        if (tail == 0)
        {
            return;
        }
        Node *node = tail;
        tail = tail->prev;
        if (tail != 0)
        {
            tail->next = 0;
        }
        else
        {
            head = 0;
        }
        delete node;
        count--;
    }

    void removeItem(T item)
    {
        Node *cur = head;
        while (cur != 0)
        {
            if (cur->data == item)
            {
                if (cur->prev != 0)
                {
                    cur->prev->next = cur->next;
                }
                else
                {
                    head = cur->next;
                }
                if (cur->next != 0)
                {
                    cur->next->prev = cur->prev;
                }
                else
                {
                    tail = cur->prev;
                }
                delete cur;
                count--;
                return;
            }
            cur = cur->next;
        }
    }
    T peekFront() const { return head == 0 ? T() : head->data; }
    Node *getHead() const { return head; }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif
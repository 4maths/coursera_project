#pragma once
#include <stdexcept>

/*
 * Doubly linked list storing pointers (e.g., Course*).
 * Supports forward/back traversal via cursor in O(1).
 */

template<typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        explicit Node(T d) : data(d), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    Node* cursor;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), cursor(nullptr) {}

    ~DoublyLinkedList() { clear(); }

    bool empty() const { return head == nullptr; }

    void push_back(T value) {
        Node* n = new Node(value);
        if (!tail) {
            head = tail = cursor = n;
        } else {
            tail->next = n;
            n->prev = tail;
            tail = n;
        }
    }

    void reset_to_head() {
        if (!head) throw std::runtime_error("Empty list");
        cursor = head;
    }

    T current() const {
        if (!cursor) throw std::runtime_error("No current");
        return cursor->data;
    }

    bool can_next() const { return cursor && cursor->next; }
    bool can_prev() const { return cursor && cursor->prev; }

    T next() {
        if (!can_next()) throw std::runtime_error("No next");
        cursor = cursor->next;
        return cursor->data;
    }

    T prev() {
        if (!can_prev()) throw std::runtime_error("No prev");
        cursor = cursor->prev;
        return cursor->data;
    }

    void clear() {
        Node* p = head;
        while (p) {
            Node* nx = p->next;
            delete p;
            p = nx;
        }
        head = tail = cursor = nullptr;
    }
};

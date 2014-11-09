//
//  list.h
//  Sudoku
//
//  Created by Jakub Trzebiatowski on 06/11/14.
//
//

#ifndef list_h
#define list_h

#include <cassert>

template<typename T>
class list {
    struct node_base {
        node_base() : prev(nullptr), next(nullptr) {}
        node_base *prev, *next;
    };
    struct node : node_base {
        node(const T& element_) : element(element_) {}
        T element;
    };
    node_base _end;
public:
    class iterator {
        node *pointer;
    public:
        iterator(node *pointer_ = nullptr) : pointer(pointer_) {}
        iterator &operator++() {
            pointer = (node*)pointer->next;
            return *this;
        }
        iterator &operator--() {
            pointer = (node*)pointer->prev;
            return *this;
        }
        T& operator*() {
            return pointer->element;
        }
        bool operator==(const iterator &it) {
            return pointer == it.pointer;
        }
        bool operator!=(const iterator &it) {
            return pointer != it.pointer;
        }
    };
    list() {
        _end.prev = &_end;
        _end.next = &_end;
    }
    bool empty() {
        return begin() == end();
    }
    void clear() {
        while(!empty())
            pop_back();
    }
    iterator begin() {
        return iterator((node*)_end.next);
    }
    iterator end() {
        return iterator((node*)&_end);
    }
    void push_back(const T& element) {
        node *new_node = new node(element);
        new_node->prev = _end.prev;
        new_node->next = &_end;
        _end.prev = new_node;
        _end.prev->next = new_node;
    }
    void pop_back() {
        assert(!empty());
        node *kill_me = (node*)_end.prev;
        _end.prev = kill_me->prev;
        kill_me->prev->next = &_end;
        delete kill_me;
    }
    void push_front(const T& element) {
        node *new_node = new node(element);
        new_node->prev = &_end;
        new_node->next = _end.next;
        _end.next->prev = new_node;
        _end.next = new_node;
    }
    void pop_front() {
        assert(!empty());
        node *kill_me = (node*)_end.next;
        kill_me->next->prev = &_end;
        _end.next = kill_me->next;
        delete kill_me;
    }
};

#endif

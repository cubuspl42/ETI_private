#ifndef list_h
#define list_h

#include <cassert>

namespace ktl {

template<typename T>
class list {
    struct node;
    struct node_base {
        node_base() : prev(nullptr), next(nullptr) {}
        node *prev, *next;
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
        _end.prev = (node*)&_end;
        _end.next = (node*)&_end;
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
        new_node->next = (node*)&_end;
        _end.prev->next = new_node;
        _end.prev = new_node;
    }
    void pop_back() {
        assert(!empty());
        node *kill_me = (node*)_end.prev;
        _end.prev = kill_me->prev;
        kill_me->prev->next = (node*)&_end;
        delete kill_me;
    }
    void push_front(const T& element) {
        node *new_node = new node(element);
        new_node->prev = (node*)&_end;
        new_node->next = _end.next;
        _end.next->prev = new_node;
        _end.next = new_node;
    }
    void pop_front() {
        assert(!empty());
        node *kill_me = (node*)_end.next;
        kill_me->next->prev = (node*)&_end;
        _end.next = kill_me->next;
        delete kill_me;
    }
};
    
}

#endif

#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <cstring>
#include <cstddef>

namespace nonstd {
    template<typename T>
    void swap(T &a, T &b) {
        T tmp(a);
        a = b;
        b = tmp;
    }

    template<typename T1, typename T2>
    struct pair
    {
        T1 first;
        T2 second;
        bool operator<(const pair &b) {
            if(this->first == b.first) {
                return this->second < b.second;
            } else return this->first < b.first;
        }
    };

    template<typename T1, typename T2>
    pair<T1, T2> make_pair(const T1 &a, const T2 &b)
    {
        pair<T1, T2> p;
        p.first = a;
        p.second = b;
        return p;
    };

    template<typename T>
    struct vector
    {
        T *storage_;
        size_t size_, reserved_;
        vector() : size_(0), reserved_(0), storage_(nullptr) {
        }
        ~vector() {
            delete[] storage_;
        }
        void clear() {
            resize(0);
        }
        void push_back(const T &value) {
            resize(size_+1);
            storage_[size_-1] = value;
        }
        void pop_back() {
            resize(size_-1);
        }
        bool empty() {
            return size_ == 0;
        }
        T *begin() {
            return storage_;
        }
        T *end() {
            return storage_ + size_;
        }
        T &back() {
            return storage_[size_-1];
        }
        void reserve(size_t size) {
            if(size > reserved_) {
                size_t new_reserved = size*2;
                T *new_storage = new T[new_reserved];
                if(storage_) {
                    memcpy(new_storage, storage_, size_*sizeof(T));
                    delete storage_;
                }
                storage_ = new_storage;
                reserved_ = new_reserved;
            }
        }
        void resize(size_t size) {
            reserve(size);
            size_ = size;
        }
        size_t size() {
            return size_;
        }
        T &operator[](size_t i) {
            return storage_[i];
        }
    };

    template <typename RandomAccessIterator>
    void push_heap (RandomAccessIterator first, RandomAccessIterator last)
    {
        size_t len = last - first;
        if(len > 1) {
            size_t element_index = len - 1;
            while(element_index){
                size_t parent_index = (element_index - 1) / 2;
                RandomAccessIterator element = first + element_index, parent = first + parent_index;
                if(*parent < *element) {
                    break;
                }
                swap(*element, *parent);
                element_index = parent_index;
            }
        }
    }

    template <typename RandomAccessIterator>
    void pop_heap (RandomAccessIterator first, RandomAccessIterator last)
    {
        size_t len = last - first;
        if(len > 1) {
            swap(*first, *--last);
            --len;
            size_t element_index = 0;
            while(true){
                RandomAccessIterator element = first + element_index;
                size_t child_index = 2 * element_index + 1;
                if(child_index >= len) {
                    break;
                }
                RandomAccessIterator child = first + child_index;
                if(child_index + 1 < len && *(child + 1) < *child) {
                    ++child_index;
                    ++child;
                }
                if(!(*element < *child)) {
                    swap(*element, *child);
                } else {
                    break;
                }
                element_index = child_index;
            }
        }
    }

    template <typename RandomAccessIterator>
    void make_heap (RandomAccessIterator first, RandomAccessIterator last)
    {
        RandomAccessIterator end = first;
        while(++end != last) {
            push_heap(first, end);
        }
    }
}

#endif

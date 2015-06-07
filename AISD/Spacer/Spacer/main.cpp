#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

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

struct Elevator {
    int yd, xd, t, to;
};

struct Field {
    int h, d;
    nonstd::vector<Elevator> elevators;
};

int width, height, ys, xs, yd, xd, num_elevators;

Field *_the_map;
Field &the_map(size_t y, size_t x) {
    return _the_map[y*width+x];
}

typedef nonstd::pair<int, nonstd::pair<int, int>> Tuple;
nonstd::vector<Tuple> heap;

Tuple make_tuple(int a, int b, int c) {
    return nonstd::make_pair(a, nonstd::make_pair(b, c));
}

int w(int ay, int ax, int by, int bx) {
    int A = the_map(ay,ax).h;
    int B = the_map(by,bx).h;
    if(B > A)
        return B - A + 1;
    else return 1;
}

void relax(int ay, int ax, int by, int bx, int new_d) {
    Field &field = the_map(by,bx);
    int &old_d = field.d;
    if(new_d < old_d) {
        old_d = new_d;
        heap.push_back(make_tuple(new_d, by, bx));
        nonstd::push_heap(heap.begin(), heap.end());
    }
}

void relax_walk(int ay, int ax, int by, int bx) {
    int new_d = the_map(ay,ax).d + w(ay, ax, by, bx);
    relax(ay, ax, by, bx, new_d);
}

void relax_elevators(int ay, int ax) {
    Field &field = the_map(ay,ax);
    auto &elevators = field.elevators;
    for(int i = 0; i < field.elevators.size(); ++i) {
        auto &e = elevators[i];
        int d = field.d, to = e.to, t = e.t;
        
        int new_d = d + (d%to ? -d%to + to + t : t);
        
        int yd = e.yd, xd = e.xd;
        relax(ay, ax, yd, xd, new_d);
    }
}

void dijkstra() {
    the_map(ys,xs).d = 0;
    heap.push_back(make_tuple(0, ys, xs));
    while(!heap.empty()) {
        nonstd::pop_heap(heap.begin(), heap.end());
        Tuple t = heap.back();
        heap.pop_back();
        int d = t.first;
        int y = t.second.first, x = t.second.second;
        if(the_map(y,x).d == d) {
            if(y > 0)
                relax_walk(y, x, y-1, x);
            if(y+1 < height)
                relax_walk(y, x, y+1, x);
            if(x > 0)
                relax_walk(y, x, y, x-1);
            if(x+1 < width)
                relax_walk(y, x, y, x+1);
            relax_elevators(y, x);
        }
    }
}

#ifdef __APPLE__
#define INPUT_FILE "/Users/kuba/Library/Developer/Xcode/DerivedData/Spacer-gcqdhcrsrjsoifbmotbkglzvhgee/Build/Products/Debug/aisd15/14.in"
#endif

int main() {
#ifdef INPUT_FILE
    FILE *file = fopen(INPUT_FILE, "r");
#else
#define file stdin
#endif
    
    fscanf(file, "%d %d %d %d %d %d %d\n", &width, &height, &xs, &ys, &xd, &yd, &num_elevators);
    _the_map = new Field[height*width];
    
    for(int i = 0; i < num_elevators; ++i) {
        int ys, xs;
        scanf("%d %d", &xs, &ys);
        Field &field = the_map(ys,xs);
        field.elevators.push_back(Elevator());
        Elevator &e = field.elevators[field.elevators.size()-1];
        fscanf(file, "%d %d %d %d", &e.xd, &e.yd, &e.t, &e.to);
    }

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            the_map(y,x).d = 1000000000;
            fscanf(file, "%d", &the_map(y,x).h);
        }
    }

    dijkstra();
    printf("%d\n", the_map(yd,xd).d);
    return 0;
}



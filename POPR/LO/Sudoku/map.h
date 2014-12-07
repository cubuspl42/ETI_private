#ifndef map_h
#define map_h

#include <cassert>
#include "algorithm.h"

namespace ktl {
    
template<typename K, typename V, unsigned max_size>
class map {
    K keys[max_size];
    V values[max_size];
    unsigned size = 0;
public:
    void clear() {
        size = 0;
    }
    V &at(const K &key) {
        K *end = keys+size;
        unsigned i = ktl::find(keys, end, key) - keys;
        if(i == size) {
            assert(size < max_size);
            keys[size++] = key;
        }
        return values[i];
    }
    inline V &operator[](const K &key) {
        return at(key);
    }
};
    
}

#endif

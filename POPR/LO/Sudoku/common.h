#ifndef common_h
#define common_h

#include "config.h"
#include <algorithm>

template<typename T>
inline bool check_bit(T n, unsigned bit) {
    return n & (1 << bit);
}

template<typename T>
inline void set_bit(T *n, unsigned bit) {
    *n |= 1 << bit;
}

template<typename T>
inline void clear_bit(T *n, unsigned bit) {
    *n &= ~(1 << bit);
}

template<typename T>
inline void toggle_bit(T *n, unsigned bit) {
    *n ^= (1 << bit);
}

template<typename T>
inline unsigned count_bits(T n) {
    unsigned c;
    for (c = 0; n; ++c)
        n &= n - 1;
    return c;
}

template<typename T>
inline bool has_single_bit(T n) {
    return n && !(n & (n - 1));
}

inline float clamp(int n, int lower, int upper) {
    return std::max(lower, std::min(n, upper));
}

#endif
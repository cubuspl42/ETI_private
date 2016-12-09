#ifndef P2_RECORD_H
#define P2_RECORD_H

#include <cstdint>
#include <iostream>

using namespace std;

struct Record {
    int64_t a0;
    int64_t a1;
    int64_t a2;
    int64_t a3;
    int64_t a4;
    int64_t x;

    int64_t pkey();
};

inline int64_t g(Record r) {
    return
            r.a0 +
            r.a1 * r.x +
            r.a2 * r.x * r.x +
            r.a3 * r.x * r.x * r.x +
            r.a4 * r.x * r.x * r.x * r.x;
}

inline bool operator<(Record a, Record b) {
    int64_t y1 = g(a);
    int64_t y2 = g(b);
    return y1 < y2;
}

inline istream &operator>>(istream &is, Record &r) {
    return is >> r.a0 >> r.a1 >> r.a2 >> r.a3 >> r.a4 >> r.x;
}

inline ostream &operator<<(ostream &os, Record r) {
    return os << "("
              << r.a0 << "," << r.a1 << "," << r.a2 << "," << r.a3 << "," << r.a4 << "," << r.x
              << "); g(r) = " << g(r);
}

#endif //P2_RECORD_H

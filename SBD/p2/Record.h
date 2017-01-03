#ifndef P2_RECORD_H
#define P2_RECORD_H

#include <cstdint>
#include <iostream>
#include <tuple>

using namespace std;

struct Record {
    int64_t a0 = 0;
    int64_t a1 = 0;
    int64_t a2 = 0;
    int64_t a3 = 0;
    int64_t a4 = 0;
    int64_t x = 0;

    Record() = default;

    Record(int64_t _a0) {
        a0 = _a0;
    }

    Record(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e, int64_t f)
        : a0(a), a1(b), a2(c), a3(d), a4(e), x(f) {}
};

inline bool operator==(Record a, Record b) {
    auto t1 = make_tuple(a.a0, a.a1, a.a2, a.a3, a.a4, a.x);
    auto t2 = make_tuple(b.a0, b.a1, b.a2, b.a3, b.a4, b.x);
    return t1 == t2;
}

inline bool operator<(Record a, Record b) {
    auto t1 = make_tuple(a.a0, a.a1, a.a2, a.a3, a.a4, a.x);
    auto t2 = make_tuple(b.a0, b.a1, b.a2, b.a3, b.a4, b.x);
    return t1 < t2;
}

inline istream &operator>>(istream &is, Record &r) {
    return is >> r.a0 >> r.a1 >> r.a2 >> r.a3 >> r.a4 >> r.x;
}

inline ostream &operator<<(ostream &os, Record r) {
    return os << "("
              << r.a0 << "," << r.a1 << "," << r.a2 << "," << r.a3 << "," << r.a4 << "," << r.x
              << ")";
}

#endif //P2_RECORD_H

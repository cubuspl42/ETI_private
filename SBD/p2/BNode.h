#ifndef P2_BNODE_H
#define P2_BNODE_H

#include "common.h"
#include "BFindResult.h"
#include "BElement.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

struct BPageHeader {
    int m = 0;
    int parent = NIL;
};

struct Ep {
    BElement e;
    int p;

    bool operator==(const Ep &o) const {
        return e == o.e && p == o.p;
    }
};

inline bool operator<(Ep e1, Ep e2) {
    return e1.e < e2.e;
}

static const size_t NODE_DATA_MEMORY_SIZE = 2 * D + 2;

struct BNode {
    int idx = NIL;
    int m = 0;
    vector<Ep> data;

    BNode() {
        data.resize(NODE_DATA_MEMORY_SIZE); // front pseudo-element + 2*D elements + extra element
    }

    BElement e(int i) const {
        assert(i > 0 && i <= m);
        return data[i].e;
    }

    void set_e(int i, BElement e) {
        assert(i > 0 && i <= m);
        data[i].e = e;
    }

    int p(int i) const {
        assert(i >= 0 && i <= m);
        return data[i].p;
    }

    auto e_begin() -> decltype(data.begin()) {
        return data.begin() + 1;
    }

    auto e_end() -> decltype(data.begin()) {
        return data.begin() + m + 1;
    }

    auto p_begin() const -> decltype(data.begin()) {
        return data.begin();
    }

    auto p_end() const -> decltype(data.begin()) {
        return data.begin() + m + 1;
    }

    int find_child(int p) {
        auto it = find_if(p_begin(), p_end(), [&](Ep ep) {
            return ep.p == p;
        });
        assert(it != p_end());
        return (int) distance(p_begin(), it);
    }

    bool is_leaf() const {
        return all_of(p_begin(), p_end(), [&](Ep ep) {
            return ep.p == NIL;
        });
    }

    bool full() {
        return m >= 2 * D;
    }

    bool overflows() const {
        return m > 2 * D;
    }

    bool underflows() const {
        return m < D;
    }

    BFindResult find(int x) {
        auto it = find_if(data.begin() + 1, data.end(), [&](Ep ep) {
            return ep.e.x == x;
        });
        if (it != data.end()) {
            return BFindResult{NIL, it->e, -1, (int) (it - data.begin())};
        } else {
            return BFindResult{NIL, {x, NOT_FOUND}, -1, NIL};
        }
    }

    void insert(BElement e) {
        assert(is_leaf());
        assert(!overflows());
        data.insert(data.begin() + m + 1, Ep{e, NIL});
        ++m;
        sort(e_begin(), e_end()); // FIXME: last element? m?
        data.resize(NODE_DATA_MEMORY_SIZE);
    }

    void psplit(int i, int lp, BElement e, int rp) {
        assert(!overflows());
        assert(i >= 0 && i <= m);
        Ep ep{e, rp};
        data.insert(data.begin() + i + 1, ep);
        data[i].p = lp;
        ++m;
        data.resize(NODE_DATA_MEMORY_SIZE);
    }

    void remove(int i) {
        assert(is_leaf());
        assert(i > 0 && i <= m);
        data.erase(data.begin() + i);
        --m;
        data.resize(NODE_DATA_MEMORY_SIZE);
    }

    void emerge(int ei, int p) {
        assert(ei > 0 && ei <= m);
        data.erase(data.begin() + ei);
        data[ei - 1].p = p;
        --m;
        data.resize(NODE_DATA_MEMORY_SIZE);
    }

    bool operator==(const BNode &o) const {
        auto begin = data.begin();
        auto end = data.begin() + m + 1;
        return m == o.m && equal(begin, end, o.data.begin());
    }

    void dump() const {
        cout << "idx: " << idx << " m: " << m << " [ ";
        for(Ep ep : data) {
            cout << "(" << ep.e.x << ", " << ep.e.a << ") " << ep.p << " ";
        }
        cout << "]";
    }
};

#endif //P2_BNODE_H

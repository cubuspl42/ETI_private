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

#if 0
struct BEntry {
    int x = 0;
    int a = NOT_FOUND;
    int p = NIL;

    BEntry() = default;

    BEntry(int _p)
            :  p(_p) {
    }

    BEntry(int _x, int _a, int _p)
            : x(_x), a(_a), p(_p) {
    }
};
#endif

#if 0
class __BPageBuf {
    vector<BElement> _e;
    vector<int> _p;
public:

    BPageBuf(vector<BElement> e, vector<int> p) : _e(move(e)), _p(move(p)) {
        assert(_p.size() == _e.size() + 1);
    }

    BPageBuf(vector<BElement> e) : _e(move(e)), _p(_e.size() + 1, NIL) {
        assert(_p.size() == _e.size() + 1);
    }

    BPageBuf() : _p{NIL} {}

    BPageBuf(BElement e0) : _e{e0}, _p{NIL, NIL} {}

    BPageBuf(int p0) : _p{p0} {}

    int m() const {
        assert(_p.size() == _e.size() + 1);
        return (int) _e.size();
    }

    BElement e(int i) const {
        assert(i > 0 && i <= m());
        return _e[i - 1];
    }

    int p(int i) const {
        assert(i >= 0 && i <= m());
        return _p[i];
    }

    tuple<BPageBuf, BElement, BPageBuf> split() {
        assert(m() >= 3);
        assert(m() % 2 == 1);
        int med = m() / 2;
        vector<BElement> le{_e.begin(), _e.begin() + med};
        vector<BElement> re{_e.begin() + med + 1, _e.end()};
        vector<int> lp{_p.begin(), _p.begin() + med + 1};
        vector<int> rp{_p.begin() + med + 1, _p.end()};
        BPageBuf lb{move(le), move(lp)};
        BPageBuf rb{move(re), move(rp)};
        BElement me = _e[med];
        return make_tuple(lb, me, rb);
    }

    /**
     * Replace pointer at index @a i with a new element with new adjacent pointers.
     * @param i index of the pointer to be replaced
     * @param x x-value of the new element
     * @param a a-value of the new element
     * @param pl left adjacent pointer of the new element
     * @param pr right adjacent pointer of the new element
     */
    void preplace(int i, BElement e, int pl, int pr) {
        assert(_p.size() > 0);
        _e.insert(_e.begin() + i, e);
        _p[i] = pl;
        _p.insert(_p.begin() + i + 1, pr);
    }

    /**
     * Insert a new element to this page.
     * @note Page needs to be a leaf node
     */
    void insert(BElement e) {
        assert(is_leaf());
        _e.push_back(e);
        _p.push_back(NIL);
        sort(_e.begin(), _e.end());
        assert(_p.size() == _e.size() + 1);
    }

    bool is_leaf() const {
        return all_of(_p.begin(), _p.end(), [&](int p) {
            return p == NIL;
        });
    }

    int find(int x) {
        auto it = find_if(_e.begin(), _e.end(), [&](BElement e) {
            return e.x == x;
        });
        if (it != _e.end()) {
            return it->a;
        } else {
            return NOT_FOUND;
        }
    }

    /**
     * @param cp child P-index
     * @return C-index of @a c
     */
    int find_child(int cp) const {
        auto it = find_if(_p.begin(), _p.end(), [&](int p) {
            return p == cp;
        });
        assert(it != _p.end());
        return (int) distance(_p.begin(), it);
    }

    void set_e(int i, BElement e) {
        assert(i > 0 && i <= m());
        _e[i - 1] = e;
        assert(is_sorted(_e.begin(), _e.end()));
    }

    const vector<BElement> &ve() const {
        return _e;
    }

    const vector<int> &vp() const {
        return _p;
    }

    void remove(int i) {
        assert(is_leaf());
        _e.erase(_e.begin() + i - 1);
        _p.pop_back();
    }

    BElement remove_max() {
        assert(is_leaf());
        BElement el = _e.back();
        _e.pop_back();
        _p.pop_back();
        return el;
    }

    void emerge(int i, int np) {
        assert(i >= 1 && i <= m());
        _e.erase(_e.begin() + i - 1);
        _p.erase(_p.begin() + i);
        _p[i - 1] = np;
    }
};
#endif

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
        return idx == o.idx && m == o.m && equal(begin, end, o.data.begin());
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

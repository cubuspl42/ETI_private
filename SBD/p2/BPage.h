#ifndef P2_BPAGE_H
#define P2_BPAGE_H

#include "common.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

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

struct BElement {
    int x = -1; // key
    int a = -1; // value

    BElement() = default;

    BElement(int _x, int _a) : x(_x), a(_a) {}
};

inline bool operator<(BElement e1, BElement e2) {
    return make_pair(e1.x, e1.a) < make_pair(e2.x, e2.a);
}

class BPageBuf {
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
};

class BPage {
    int _idx = NIL;
    int _parent = NIL;
    BPageBuf _buf;
public:
    BPage(BPage &&) = default;

    BPage(const BPage &) = delete;

    BPage(int idx, int parent, BPageBuf buf);

    void reset(int parent, BPageBuf buf);

    void reset(BPageBuf buf);

    /**
     * P-index of this page
     */
    int idx() const {
        return _idx;
    }

    int parent() const {
        return _parent;
    }

    BPageBuf &buf() {
        return _buf;
    }

    const BPageBuf &buf() const {
        return _buf;
    }
};

#endif //P2_BPAGE_H

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

struct BEntry {
    int x = 0;
    int a = NOT_FOUND;
    int p = NIL;

    BEntry() = default;

    BEntry(int _x, int _a, int _p)
            : x(_x), a(_a), p(_p) {
    }
};

inline bool operator<(BEntry a, BEntry b) {
    return a.x < b.x;
}

class BPage {
    int _idx = NIL;
    int _parent = NIL;
    vector<BEntry> _data;
public:
    BPage(BPage &&) = default;
    BPage(const BPage&) = delete;

    BPage(int idx, int parent, vector<BEntry> data);

    void reset(int parent, vector<BEntry> data);

    /**
     * P-index of this page
     */
    int idx() const {
        return _idx;
    }

    int m() const {
        assert(_data.size() > 1);
        return (int) _data.size() - 1;
    }

    int parent() const {
        return _parent;
    }

    int a(int i) const {
        return _data[i].a;
    }

    int x(int i) const {
        assert(i >= 1 && i <= m());
        return _data[i].x;
    }

    int p(int i) const {
        assert(i >= 0 && i <= m());
        return _data[i].p;
    }

    int find(int x) const {
        auto it = find_if(_data.begin() + 1, _data.end(), [&](BEntry e) {
            return e.x == x;
        });
        if(it != _data.end()) {
            return it->a;
        } else {
            return NOT_FOUND;
        }
    }

    void insert(BEntry e);

    /**
     * @param c child P-index
     * @return C-index of @a c
     */
    int find_child(int c) const {
        return NIL; // FIXME
    }

    tuple<vector<BEntry>, BEntry, vector<BEntry>> split();
};

#endif //P2_BPAGE_H

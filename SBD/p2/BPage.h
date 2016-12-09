#ifndef P2_BPAGE_H
#define P2_BPAGE_H

#import "common.h"

#include <cassert>
#include <string>
#include <vector>

using namespace std;

class BPage {
    vector<int> data;
    int _idx = NIL;
    int _m = 0;
    int _parent = NIL;
public:
    /**
     * P-index of this page
     */
    int idx() {
        return _idx;
    }

    int m() {
        return _m;
    }

    int parent() {
        return _parent;
    }

    int a(int i) {
        return data[1 + i * 3 + 2];
    }

    int p(int i) {
        assert(i >= 0 && i <= _m);
        return 0;
    }

    int x(int i) {
        assert(i >= 1 && i <= _m);
        return 0;
    }

    int find(int x) {
        return NOT_FOUND; // FIXME
    }

    void insert(int x, int a) { // FIXME
    }

    /**
     * @param c child P-index
     * @return C-index of @a c
     */
    int find_child(int c) {
        return NIL; // FIXME
    }
};

#endif //P2_BPAGE_H

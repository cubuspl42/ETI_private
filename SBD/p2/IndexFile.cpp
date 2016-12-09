//
// Created by kuba on 09.12.16.
//

#include "IndexFile.h"
#include "BPage.h"

pair<int, int> IndexFile::_find(int p, int x) {
    if(p == NIL) {
        return make_pair(p, NOT_FOUND);
    }

    BPage pg = pgf.read_page(p);
    int m = pg.m();

    int a = pg.find(x);
    if(a != NOT_FOUND) {
        return make_pair(p, a);
    }

    for(int i = 1; i <= m; ++i) {
        int xi = pg.x(i);
        if(x < xi) {
            int pi1 = pg.p(i - 1);
            return _find(pi1, x);
        }
    }

    int xm = pg.x(m);
    assert(x > xm);
    int pm = pg.p(m);
    return _find(pm, x);
}


int IndexFile::find(int x) {
    return _find(s, x).second;
}

InsertStatus IndexFile::_insert(int p, int x, int a) {
    assert(p >= 0);

    BPage pg = pgf.read_page(p);
    int p0m = pg.m();

    if(p0m < 2 * d) {
        pg.insert(x, a);
        return OK;
    } else {
        assert(p0m == 2 * d);
        if(_compensate(p) == COMPENSATE_OK) {
            return OK;
        } else {
            int parent = pg.parent();
            if(parent != NIL) {
                _split(p);
                return _insert(parent, x, a);
            }
        }
    }
}

InsertStatus IndexFile::insert(int x, int a) {
    int p, ao;
    std::tie(p, ao) = _find(s, x);

    if(ao != NOT_FOUND) {
        assert(ao == a);
        return ALREADY_EXISTS;
    }

    return _insert(p, x, a);
}

void IndexFile::remove(int x) {
    // FIXME
}

static void distribute(BPage lpg, BPage rpg, BPage page, int i) {

}

CompensateStatus IndexFile::_compensate(int p) {
    assert(p >= 0);
    BPage pg = pgf.read_page(p);
    int parent = pg.parent();
    if(parent == NIL) {
        return COMPENSATE_NOT_POSSIBLE;
    }

    BPage ppg = pgf.read_page(parent);
    int pm = ppg.m();
    int c = ppg.find_child(p); // C-index of @p in its parent

    BPage lpg, rpg;
    if(c - 1 >= 0) {
        int lp = ppg.p(c - 1); // P-index of left sibling
        lpg = pgf.read_page(lp);
    }
    if(c + 1 <= pm) {
        int rp = ppg.p(c + 1); // P-index of right sibling
        rpg = pgf.read_page(rp);
    }

    int lpm = lpg.m(), rpm = rpg.m();
    if(lpm < d * 2) {
        distribute(lpg, pg, ppg, c);
        return COMPENSATE_OK;
    } else if(rpm < d * 2) {
        distribute(pg, rpg, ppg, c + 1);
    } else {
        return COMPENSATE_NOT_POSSIBLE;
    }
}

void IndexFile::_split(int p) {

}


//
// Created by kuba on 09.12.16.
//

#include "IndexFile.h"
#include "BPage.h"

pair<int, int> IndexFile::_find(int p, int x) {
    BPage &pg = pgf.read_page(p);
    int m = pg.m();

    int a = pg.find(x);
    if (a != NOT_FOUND) {
        return make_pair(p, a);
    }

    for (int i = 1; i <= m; ++i) {
        int xi = pg.x(i);
        if (x < xi) {
            int pi1 = pg.p(i - 1);
            if (pi1 == NIL) {
                return make_pair(p, NOT_FOUND);
            } else {
                return _find(pi1, x);
            }
        }
    }

    int xm = pg.x(m);
    assert(x > xm);
    int pm = pg.p(m);
    if (pm == NIL) {
        return make_pair(p, NOT_FOUND);
    } else {
        return _find(pm, x);
    }
}


int IndexFile::find(int x) {
    if (s == NIL) {
        return NOT_FOUND;
    }
    return _find(s, x).second;
}

void IndexFile::_insert(int p, BEntry e) {
    assert(p >= 0);
    BPage &pg = pgf.read_page(p);
    pg.insert(e);

    if (pg.m() > 2 * d) {
        assert(pg.m() == 2 * d + 1);
        if (_compensate(p) != COMPENSATE_OK) {
            _split(p);
        }
    }
}

InsertStatus IndexFile::insert(int x, int a) {
    if (s == NIL) {
        BPage &spg = pgf.make_page(NIL);
        s = spg.idx();
        spg.insert(BEntry{x, a, NIL});
        return OK;
    }

    int p, ao;
    std::tie(p, ao) = _find(s, x);

    if (ao != NOT_FOUND) {
        assert(ao == a);
        return ALREADY_EXISTS;
    }

    _insert(p, {x, a, NIL});
    return OK;
}

void IndexFile::remove(int x) {
    // FIXME
}

static void distribute(BPage &lpg, BPage &rpg, BPage &page, int i) {

}

CompensateStatus IndexFile::_compensate(int p) {
    assert(p >= 0);
    BPage &pg = pgf.read_page(p);
    int parent = pg.parent();
    if (parent == NIL) {
        return COMPENSATE_NOT_POSSIBLE;
    }

    BPage &ppg = pgf.read_page(parent);
    int pm = ppg.m();
    int c = ppg.find_child(p); // C-index of @p in its parent

    BPage *lpg = nullptr, *rpg = nullptr;
    if (c - 1 >= 0) {
        int lp = ppg.p(c - 1); // P-index of left sibling
        lpg = &pgf.read_page(lp);
    }
    if (c + 1 <= pm) {
        int rp = ppg.p(c + 1); // P-index of right sibling
        rpg = &pgf.read_page(rp);
    }

    if (lpg && lpg->m() < d * 2) {
        distribute(*lpg, pg, ppg, c);
        return COMPENSATE_OK;
    } else if (rpg && rpg->m() < d * 2) {
        distribute(pg, *rpg, ppg, c + 1);
        return COMPENSATE_OK;
    } else {
        return COMPENSATE_NOT_POSSIBLE;
    }
}

void IndexFile::_split(int p) {
    BPage &pg = pgf.read_page(p);
    assert(pg.m() > 2 * d);
    int parent = pg.parent();
    BPage &ppg = parent == NIL ?
                 pgf.make_page(NIL) :
                 pgf.read_page(parent); // FIXME: dangling reference...
    vector<BEntry> ldata, rdata;
    BEntry med;
    tie(ldata, med, rdata) = pg.split();
    pg.reset(ppg.idx(), ldata);
    BPage &rpg = pgf.make_page(ppg.idx(), rdata);
    med.p = rpg.idx();
    _insert(ppg.idx(), med);
}

void IndexFile::_for_each(int p, function<void(pair<int, int>)> f) {
    if (p == NIL) {
        return;
    }

    BPage &pg = pgf.read_page(p);
    assert(pg.m() > 0);

    int p0 = pg.p(0);
    _for_each(p0, f);

    for (int i = 1; i <= pg.m(); ++i) {
        int x = pg.x(i);
        int a = pg.a(i);
        pair<int, int> xa = make_pair(x, a);
        f(xa);
        int pi = pg.p(i);
        _for_each(pi, f);
    }
}

void IndexFile::for_each(function<void(pair<int, int>)> f) {
    _for_each(s, f);
}



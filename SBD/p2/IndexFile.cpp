//
// Created by kuba on 09.12.16.
//

#include "IndexFile.h"
#include "BPage.h"

#include <iostream>

pair<int, int> IndexFile::_find(int p, int x) {
    BPageBuf &pgb = pgf.read_page(p).buf();
    int m = pgb.m();

    int a = pgb.find(x);
    if (a != NOT_FOUND) {
        return make_pair(p, a);
    }

    for (int i = 1; i <= m; ++i) {
        int xi = pgb.e(i).x;
        if (x < xi) {
            int pi1 = pgb.p(i - 1);
            if (pi1 == NIL) {
                return make_pair(p, NOT_FOUND);
            } else {
                return _find(pi1, x);
            }
        }
    }

    int xm = pgb.e(m).x;
    assert(x > xm);
    int pm = pgb.p(m);
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

void IndexFile::_insert(int p, BElement e) {
    assert(p >= 0);
    BPage &pg = pgf.read_page(p);
    pg.buf().insert(e);

    if (page_overflows(pg)) {
        assert(pg.buf().m() == 2 * d + 1);
        if (_compensate(p) != COMPENSATE_OK) {
            _split(p);
        }
    }

//    assert(!page_overflows(pg));
}

InsertStatus IndexFile::insert(int x, int a) {
    if (s == NIL) {
        make_root(BPageBuf(BElement(x, a)));
        return OK;
    }

    int p, ao;
    std::tie(p, ao) = _find(s, x);

    if (ao != NOT_FOUND) {
        assert(ao == a);
        return ALREADY_EXISTS;
    }

    _insert(p, BElement{x, a});
    return OK;
}

void IndexFile::remove(int x) {
    // FIXME
}

static void distribute(BPage &lpg, BPage &rpg, BPage &ppg, int i) {
    vector<BElement> e;
    BPageBuf &lpgb = lpg.buf();
    BPageBuf &ppgb = ppg.buf();
    BPageBuf &rpgb = rpg.buf();
    for(int j = 1; j <= lpgb.m(); ++j) {
        e.push_back(lpgb.e(j));
    }
    e.push_back(ppgb.e(i));
    for(int j = 1; j <= rpgb.m(); ++j) {
        e.push_back(rpgb.e(j));
    }
    assert(e.size() >= 3);

    int m = (int) e.size() / 2;
    BElement me = e[m];

    vector<BElement> le{e.begin(), e.begin() + m};
    vector<BElement> re{e.begin() + m + 1, e.end()};

    BPageBuf lb{le};
    BPageBuf rb{re};

    lpg.reset(lb);
    ppgb.set_e(i, me);
    rpg.reset(rb);
}

CompensateStatus IndexFile::_compensate(int p) {
    assert(p >= 0);
    BPage &pg = pgf.read_page(p);
    int parent = pg.parent();
    if (parent == NIL) {
        return COMPENSATE_NOT_POSSIBLE;
    }

    BPage &ppg = pgf.read_page(parent);
    BPageBuf &ppgb = ppg.buf();
    int pm = ppgb.m();
    int c = ppgb.find_child(p); // C-index of @p in its parent

    BPage *lpg = nullptr, *rpg = nullptr;
    if (c - 1 >= 0) {
        int lp = ppgb.p(c - 1); // P-index of left sibling
        lpg = &pgf.read_page(lp);
    }
    if (c + 1 <= pm) {
        int rp = ppgb.p(c + 1); // P-index of right sibling
        rpg = &pgf.read_page(rp);
    }

    if (lpg && lpg->buf().m() < d * 2) {
        distribute(*lpg, pg, ppg, c);
        return COMPENSATE_OK;
    } else if (rpg && rpg->buf().m() < d * 2) {
        distribute(pg, *rpg, ppg, c + 1);
        return COMPENSATE_OK;
    } else {
        return COMPENSATE_NOT_POSSIBLE;
    }
}

void IndexFile::_split(int p) {
    BPage &pg = pgf.read_page(p);
    BPageBuf &pgb = pg.buf();
    assert(pgb.m() > 2 * d);
    int parent = pg.parent();
    BPage &ppg = parent == NIL ?
                 make_root(BPageBuf{NIL}) :
                 pgf.read_page(parent);
    BPageBuf &ppgb = ppg.buf();
    int c = (parent == NIL) ? 0 : ppgb.find_child(pg.idx());

    BPageBuf lb, rb;
    BElement med;
    tie(lb, med, rb) = pgb.split();
    pg.reset(ppg.idx(), lb);
    BPage &rpg = pgf.make_page(ppg.idx(), rb);
    ppgb.preplace(c, med, pg.idx(), rpg.idx());

    if(page_overflows(ppg)) {
        _split(ppg.idx());
    }
}

void IndexFile::_for_each(int p, function<void(pair<int, int>)> f) {
    if (p == NIL) {
        return;
    }

    BPage &pg = pgf.read_page(p);
    BPageBuf &pgb = pg.buf();
    assert(pgb.m() > 0);

    int p0 = pgb.p(0);
    _for_each(p0, f);

    for (int i = 1; i <= pgb.m(); ++i) {
        int x = pgb.e(i).x;
        int a = pgb.e(i).a;
        pair<int, int> xa = make_pair(x, a);
        f(xa);
        int pi = pgb.p(i);
        _for_each(pi, f);
    }
}

void IndexFile::for_each(function<void(pair<int, int>)> f) {
    _for_each(s, f);
}

bool IndexFile::page_overflows(BPage &pg) {
    return pg.buf().m() > 2 * d;
}

BPage &IndexFile::make_root(BPageBuf buf) {
    BPage &pg = pgf.make_page(NIL, move(buf));
    s = pg.idx();
    return pg;
}

void IndexFile::_dump(int p) {
    BPage &pg = pgf.read_page(p);
    BPageBuf &pgb = pg.buf();
    assert(pgb.m() > 0);
    cout << pg.idx() << ": [" << pgb.p(0);
    for(int i = 1; i <= pgb.m(); ++i) {
        cout << " (" << pgb.e(i).x << ") " << pgb.p(i);
    }
    cout << "]" << endl;

    for(int i = 0; i <= pgb.m(); ++i) {
        int pi = pgb.p(i);
        if(pi > -1) {
            _dump(pi);
        }
    }
}

void IndexFile::dump() {
    _dump(s);
}



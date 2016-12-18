//
// Created by kuba on 09.12.16.
//

#include "BTree.h"
#include "BNode.h"

#include <iostream>
#include <stack>

pair<int, int> BTree::_find(BStorage &stg, vector<BNode> &mem, int lv, int p, int x) {
    BNode &nd = mem[lv];
    stg.read_page(nd, p);
    int m = nd.m;

    int a = nd.find(x);
    if (a != NOT_FOUND) {
        return make_pair(p, a);
    }

    for (int i = 1; i <= m; ++i) {
        int xi = nd.e(i).x;
        if (x < xi) {
            int pi1 = nd.p(i - 1);
            if (pi1 == NIL) {
                return make_pair(p, NOT_FOUND);
            } else {
                return _find(stg, mem, lv + 1, pi1, x);
            }
        }
    }

    int xm = nd.e(m).x;
    assert(x > xm);
    int pm = nd.p(m);
    if (pm == NIL) {
        return make_pair(p, NOT_FOUND);
    } else {
        return _find(stg, mem, lv + 1, pm, x);
    }
}


int BTree::find(int x) {
    if (hdr.s == NIL) {
        return NOT_FOUND;
    }
    int rv = _find(_stg, _mem, 0, hdr.s, x).second;
    return rv;
}

void BTree::_fix_overflow(BStorage &stg, vector<BNode> &mem, int lv) {
    BNode &nd = mem[lv];

    if (nd.overflows()) {
        assert(nd.m == 2 * D + 1);
        BNode &exnd = mem.back(); // extra node

        if (lv > 0) {
            BNode &pnd = mem[lv - 1];
            if(_compensate(stg, nd, pnd, exnd) == COMPENSATE_OK) {
                stg.write_page(nd);
                stg.write_page(pnd);
                stg.write_page(exnd);
                return;
            }
        }

        int nnd_idx = hdr.n++;
        exnd.idx = nnd_idx;

        BElement me = _split(nd, exnd);

        _stg.write_page(nd);
        _stg.write_page(exnd);

        if(lv == 0) {
            exnd.idx = hdr.n++;
            exnd.m = 1;
            exnd.data[0] = Ep{BElement{-1, -1}, nd.idx};
            exnd.data[1] = Ep{me, nnd_idx};
            hdr.s = exnd.idx;
            _stg.write_header(hdr);
            _stg.write_page(exnd);
        } else {
            BNode &pnd = mem[lv - 1];
            int c = pnd.find_child(nd.idx);
            pnd.psplit(c, nd.idx, me, nnd_idx);

            if(pnd.overflows()) {
                _fix_overflow(stg, mem, lv - 1);
            }
        }
    }

    assert(!nd.overflows());
    _stg.write_page(nd);
}

InsertStatus BTree::insert(int x, int a) {
    if (hdr.s == NIL) {
        BNode &root = _mem[0];
        root.idx = hdr.n++;
        root.m = 1;
        root.data[0] = Ep{BElement{-1, -1}, NIL};
        root.data[1] = Ep{BElement{x, a}, NIL};
        hdr.s = root.idx;
        _stg.write_header(hdr);
        _stg.write_page(root);
        return OK;
    } else {
        int p, ao;
        tie(p, ao) = _find(_stg, _mem, 0, hdr.s, x);

        if (ao != NOT_FOUND) {
            assert(ao == a);
            return ALREADY_EXISTS;
        }

        BNode &nd = _mem[hdr.h - 1];
        nd.insert(BElement{x, a});

        _fix_overflow(_stg, _mem, hdr.h - 1);
        return OK;
    }
}

#if 0
static BNode &max_leaf(PagedFile &pgf, int p) {
    BNode &pg = pgf.read_page(p);
    BPageBuf &pgb = pg.buf();
    if(pgb.is_leaf()) {
        return pg;
    } else {
        int rp = pgb.p(pgb.m());
        return max_leaf(pgf, rp);
    }
}


void BTree::remove(int x) {
    int p, ao;
    tie(p, ao) = _find(_stg, _mem, 0, s, x);
    assert(ao != NOT_FOUND);
    BNode &pg = pgf.read_page(p);
    BPageBuf &pgb = pg.buf();

    int c = pgb.find_child(p);
    if(pgb.is_leaf()) {
        pgb.remove(c);
        _fix_leaf(pg);
    } else {
        int lp = pgb.p(c - 1);
        BNode &ml = max_leaf(pgf, lp);
        BPageBuf &mlb = ml.buf();
        BElement e = mlb.remove_max();
        pgb.set_e(c, e);
        _fix_leaf(ml);
    }
}

#endif
#if 0
void BTree::_fix_leaf(BNode &pg) {
    if(page_underflows(pg)) {
        if(_compensate(<#initializer#>, <#initializer#>, <#initializer#>, <#initializer#>) == COMPENSATE_NOT_POSSIBLE) {
            BNode &ppg = pgf.read_page(pg.parent());
            BPageBuf &ppgb = ppg.buf();
            int c = ppgb.find_child(pg.idx());
            if(c < ppgb.m()) {
                int rs = ppgb.p(c + 1);
                BNode &rp = pgf.read_page(rs);
                _merge(pg, rp, ppgb, c);
            } else {
                int ls = ppgb.p(c - 1);
                BNode &lp = pgf.read_page(ls);
                _merge(lp, pg, ppgb, c);
            }
        }
    }
}

void BTree::_merge(BNode &lp, BNode &rp, BPageBuf &ppgb, int i) {
    BPageBuf &lpb = lp.buf();
    BPageBuf &rpb = rp.buf();
    BElement el = ppgb.e(i);
    vector<BElement> ve = lpb.ve();
    ve.push_back(el);
    ve.insert(ve.end(), rpb.ve().begin(), rpb.ve().end());
    vector<int> vp = lpb.vp();
    vp.insert(vp.end(), rpb.vp().begin(), rpb.vp().end());
    BPageBuf buf{ve, vp};
    ppgb.emerge(i, lp.idx());
    // FIXME: Garbage collect @rp
}
#endif

static void distribute_l(BNode &lnd, BNode &rnd, BNode &pnd, int i) {
    assert(false);

    assert(lnd.overflows());
    assert(!rnd.full());

    int sm = lnd.m + 1 + rnd.m;
    int lm = sm / 2;
    int rm = sm - lm - 1;

    BElement me = pnd.e(i);
    rnd.data.front().e = me;

    rnd.data.insert(rnd.data.begin(), lnd.data.begin() + lm + 1, lnd.data.end());
    BElement nme = rnd.data.front().e;
    pnd.set_e(i, nme);

    lnd.m = lm;
    rnd.m = rm;
}

static void distribute_r(BNode &lnd, BNode &rnd, BNode &pnd, int i) {
    assert(lnd.overflows());
    assert(!rnd.full());

    int sm = lnd.m + 1 + rnd.m;
    int lm = sm / 2;
    int rm = sm - lm - 1;

    BElement me = pnd.e(i);
    rnd.data.front().e = me;

    rnd.data.insert(rnd.data.begin(), lnd.e_begin() + lm, lnd.e_end());
    BElement nme = rnd.data.front().e;
    pnd.set_e(i, nme);

    lnd.m = lm;
    rnd.m = rm;
}

CompensateStatus BTree::_compensate(BStorage &stg, BNode &nd, BNode &pnd, BNode &snd) {
    int c = pnd.find_child(nd.idx); // C-index of @p in its parent

    if(c > 0) {
        /* Try left sibling */
        int ls = pnd.p(c - 1);
        stg.read_page(snd, ls);
        if(!snd.full()) {
            distribute_l(snd, nd, pnd, c);
            return COMPENSATE_OK;
        }
    }

    if(c < pnd.m) {
        /* Try right sibling */
        int rs = pnd.p(c + 1);
        stg.read_page(snd, rs);
        if(!snd.full()) {
            distribute_r(nd, snd, pnd, c + 1);
            return COMPENSATE_OK;
        }
    }

    /* Give up */
    return COMPENSATE_NOT_POSSIBLE;
}

BElement BTree::_split(BNode &nd, BNode &nnd) {
    assert(nd.m == 2 * D + 1);
    copy(nd.data.begin() + D + 1, nd.data.end(), nnd.data.begin());
    nd.m = nnd.m = D;
    BElement me = nd.data[D + 1].e;
    return me;
}

void BTree::_for_each(int p, vector<BNode> &mem, int lv, function<void(BElement)> f) {
    if (p == NIL) {
        return;
    }

    BNode &nd = mem[lv];
    _stg.read_page(nd, p);
    assert(nd.m > 0);

    int p0 = nd.p(0);
    _for_each(p0, mem, lv + 1, f);

    for (int i = 1; i <= nd.m; ++i) {
        BElement e = nd.e(i);
        f(e);
        int pi = nd.p(i);
        _for_each(pi, mem, lv + 1, f);
    }
}

void BTree::for_each(function<void(BElement)> f) {
    _for_each(hdr.s, _mem, 0, f);
}


void BTree::_dump(int p, int lv) {
    BNode &nd = _mem[lv];
    _stg.read_page(nd, p);
    assert(nd.m > 0);
    assert(nd.idx == p);
    cout << "{" << lv << "} " << p << ": [";
    for(int i = 0; i <= nd.m; ++i) {
        if(i > 0) {
            int x = nd.e(i).x;
            cout << " (" << x << ") ";
        }
        int pi = nd.p(i);
        if(pi >= 0) {
            cout << pi;
        } else {
            cout << ".";
        }
    }
    cout << "]" << endl;

    for(int i = 0; i <= nd.m; ++i) {
        int pi = nd.p(i);
        if(pi > -1) {
            _dump(pi, lv + 1);
        }
    }
}

void BTree::dump() {
    _dump(hdr.s, 0);
}

BTree::BTree(BStorage &stg) : _stg{stg} {
    hdr = stg.read_header();
    _mem.resize(hdr.h + 1);
}

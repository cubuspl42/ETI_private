//
// Created by kuba on 09.12.16.
//

#include "BTree.h"
#include "BNode.h"
#include "BFindResult.h"
#include "BElement.h"

BFindResult BTree::_find(BTreeStorage &stg, vector<BNode> &mem, int lv, int p, int x) {
    BNode &nd = mem[lv];
    stg.read_page(nd, p);
    int m = nd.m;

    auto nfr = nd.find(x);
    if (nfr.e.a != NOT_FOUND) {
        return BFindResult{p, nfr.e, lv, nfr.c};
    }

    for (int i = 1; i <= m; ++i) {
        int xi = nd.e(i).x;
        if (x < xi) {
            int pi1 = nd.p(i - 1);
            if (pi1 == NIL) {
                return BFindResult{p, {x, NOT_FOUND}, lv, NIL};
            } else {
                return _find(stg, mem, lv + 1, pi1, x);
            }
        }
    }

    int xm = nd.e(m).x;
    assert(x > xm);
    int pm = nd.p(m);
    if (pm == NIL) {
        return BFindResult{p, {x, NOT_FOUND}, lv, NIL};
    } else {
        return _find(stg, mem, lv + 1, pm, x);
    }
}

BFindResult BTree::_find_max(BTreeStorage &stg, vector<BNode> &mem, int lv, int p) {
    BNode &nd = mem[lv];
    stg.read_page(nd, p);

    if(nd.is_leaf()) {
        int ei = nd.m;
        BElement mxe = nd.e(ei);
        return BFindResult{nd.idx, mxe, lv, ei};
    } else {
        int mxp = nd.p(nd.m);
        return _find_max(stg, mem, lv + 1, mxp);
    }
}

int BTree::find(int x) {
    if (hdr.s == NIL) {
        return NOT_FOUND;
    }
    _resize_mem();
    auto rv = _find(_stg, _mem, 0, hdr.s, x);
    return rv.e.a;
}

void BTree::_fix_overflow(BTreeStorage &stg, vector<BNode> &mem, int lv) {
    BNode &nd = mem[lv];

    if (nd.overflows()) {
        assert(nd.m == 2 * D + 1);
        BNode &exnd = mem.back(); // extra node

        if (lv > 0) { // TODO: change if's order?
            BNode &pnd = mem[lv - 1];
            if (_compensate(stg, nd, pnd, exnd) == COMPENSATE_OK) { // TODO: exnd inside compensate?
                stg.write_page(nd);
                stg.write_page(pnd);
                stg.write_page(exnd);
                return;
            }
        }

        int nnd_idx = hdr.n++; // TODO: -> allocate node
        exnd.idx = nnd_idx;

        BElement me = _split(nd, exnd);

        _stg.write_page(nd); // TODO: Minimize page writes?
        _stg.write_page(exnd);
        _stg.write_header(hdr); // TODO: When to write header?

        if (lv == 0) { // TODO: Refactor -> _grow_tree
            exnd.idx = hdr.n++;
            exnd.m = 1;
            exnd.data[0] = Ep{BElement{-1, -1}, nd.idx};
            exnd.data[1] = Ep{me, nnd_idx};
            hdr.s = exnd.idx;
            ++hdr.h; // TODO: Extract! Resize mem?
            _stg.write_header(hdr);
            _stg.write_page(exnd);
        } else {
            BNode &pnd = mem[lv - 1];
            int c = pnd.find_child(nd.idx);
            pnd.psplit(c, nd.idx, me, nnd_idx);

            _fix_overflow(stg, mem, lv - 1);
        }
    }

    assert(!nd.overflows());
    _stg.write_page(nd);
}

void BTree::_fix_underflow(BTreeStorage &stg, std::vector<BNode> &mem, int lv) {
    BNode &nd = mem[lv];

    if(lv > 0) {
        if(nd.underflows()) {
            assert(nd.m == D - 1);

            BNode &exnd = mem.back(); // extra node

            BNode &pnd = mem[lv - 1];
            if (_compensate(stg, nd, pnd, exnd) == COMPENSATE_OK) {
                stg.write_page(nd);
                stg.write_page(pnd);
                stg.write_page(exnd);
                return;
            }

            int pi = pnd.find_child(nd.idx);


            if(pi > 0) {
                assert(pi > 0);
                int pl = pnd.p(pi - 1);
                int ei = pi;
                stg.read_page(exnd, pl);
                _merge(exnd, nd, pnd, ei);
                // TODO: write exnd here?
            } else {
                assert(pi < pnd.m);
                int pr = pnd.p(pi + 1);
                int ei = pi + 1;
                stg.read_page(exnd, pr);
                _merge(nd, exnd, pnd, ei);
                // TODO: write exnd here?
            }

            _fix_underflow(stg, mem, lv - 1);
        }
    } else {
        if(nd.m == 0) {
            _shrink(nd);
        }
    }

    _stg.write_page(nd);
}

InsertStatus BTree::insert(int x, int a) {
    _resize_mem();
    if (hdr.s == NIL) { // TODO: Refactor -> _grow
        BNode &root = _mem[0];
        root.idx = hdr.n++;
        ++hdr.h;
        root.m = 1;
        root.data[0] = Ep{BElement{-1, -1}, NIL};
        root.data[1] = Ep{BElement{x, a}, NIL};
        hdr.s = root.idx;
        _stg.write_header(hdr);
        _stg.write_page(root);
        return OK;
    } else {
        auto fr = _find(_stg, _mem, 0, hdr.s, x);

        if (fr.e.a != NOT_FOUND) {
            assert(fr.e.a == a);
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


#endif

int BTree::remove(int x) {
    _resize_mem();
    auto fr = _find(_stg, _mem, 0, hdr.s, x);
    assert(fr.e.a != NOT_FOUND);

    BNode &nd = _mem[fr.lv];

    if (fr.lv != hdr.h - 1) {
        int lp = nd.p(fr.c - 1);
        auto frm = _find_max(_stg, _mem, fr.lv + 1, lp);
        assert(frm.lv == hdr.h - 1);

        nd.set_e(fr.c, frm.e);
        _stg.write_page(nd);

        BNode &mnd = _mem[hdr.h - 1];
        mnd.remove(frm.c);
        _fix_underflow(_stg, _mem, frm.lv);
        return frm.e.a;
    } else {
        nd.remove(fr.c);
        _fix_underflow(_stg, _mem, fr.lv);
        return fr.e.a;
    }
}

int BTree::update(int x, int na) {
    _resize_mem();
    auto fr = _find(_stg, _mem, 0, hdr.s, x);
    assert(fr.e.a != NOT_FOUND);
    BNode &nd = _mem[fr.lv];
    nd.set_e(fr.c, BElement{x, na});
    _stg.write_page(nd);
    return fr.e.a;
}

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
#endif

static bool can_compensate(BNode &nda, BNode &ndb) {
    int sm = nda.m + ndb.m;
    return sm >= 2 * D && sm <= 4 * D;
}

static void distribute_l(BNode &lnd, BNode &rnd, BNode &pnd, int i) {
    assert(can_compensate(lnd, rnd));

    int sm = lnd.m + 1 + rnd.m;
    int lm = sm / 2;
    int rm = sm - lm - 1;

    BElement me = pnd.e(i);
    rnd.data.front().e = me;

    auto l1 = lnd.data.begin() + lnd.m + 1;
    auto r0 = rnd.data.begin();
    auto r1 = r0 + (lm - lnd.m);
    auto r2 = r0 + rnd.m + 1;

    copy(r0, r1, l1);
    copy(r1, r2, r0);

    BElement nme = rnd.data.front().e;
    pnd.set_e(i, nme);

    lnd.data.front().e = BElement{};
    rnd.data.front().e = BElement{};

    lnd.m = lm;
    rnd.m = rm;
}

static void distribute_r(BNode &lnd, BNode &rnd, BNode &pnd, int i) {
    assert(can_compensate(lnd, rnd));

    int sm = lnd.m + 1 + rnd.m;
    int lm = sm / 2;
    int rm = sm - lm - 1;

    BElement me = pnd.e(i);
    rnd.data.front().e = me;

    rnd.data.insert(rnd.data.begin(), lnd.e_begin() + lm, lnd.e_end());
    rnd.data.resize(NODE_DATA_MEMORY_SIZE);

    BElement nme = rnd.data.front().e;
    pnd.set_e(i, nme);

    lnd.data.front().e = BElement{};
    rnd.data.front().e = BElement{};

    lnd.m = lm;
    rnd.m = rm;
}

static void distribute(BNode &lnd, BNode &rnd, BNode &pnd, int i) {
    assert(lnd.m != rnd.m);
    if(rnd.m > lnd.m) {
        distribute_l(lnd, rnd, pnd, i);
    } else {
        distribute_r(lnd, rnd, pnd, i);
    }
}

CompensateStatus BTree::_compensate(BTreeStorage &stg, BNode &nd, BNode &pnd, BNode &snd) {
    int c = pnd.find_child(nd.idx); // C-index of @p in its parent

    if (c > 0) {
        /* Try left sibling */
        int ls = pnd.p(c - 1);
        stg.read_page(snd, ls);
        if (can_compensate(snd, nd)) {
            distribute(snd, nd, pnd, c);
            return COMPENSATE_OK;
        }
    }

    if (c < pnd.m) {
        /* Try right sibling */
        int rs = pnd.p(c + 1);
        stg.read_page(snd, rs);
        if (can_compensate(snd, nd)) {
            distribute(nd, snd, pnd, c + 1);
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
    nnd.data.front().e = BElement{};
    return me;
}

void BTree::_merge(BNode &lp, BNode &rp, BNode &pnd, int ei) {
    BElement pe = pnd.e(ei);
    rp.data.front().e = pe;

    auto l1 = lp.data.begin() + lp.m + 1;
    auto r1 = rp.data.begin();
    auto r2 = r1 + rp.m + 1;
    copy(r1, r2, l1);

    rp.data.front().e = BElement{};

    lp.m = lp.m + 1 + rp.m;
    rp.m = 0;

    pnd.emerge(ei, lp.idx);

    _stg.write_page(lp); // TOOD: Minimize page writes?
    _stg.write_page(rp);

    // TODO: Delete @rp

#if 0
    BPageBuf &lpb = lp.buf();
    BPageBuf &rpb = rp.buf();
    BElement el = ppgb.e(i);
    vector<BElement> ve = lpb.ve();
    ve.push_back(el);
    ve.insert(ve.end(), rpb.ve().begin(), rpb.ve().end());
    vector<int> vp = lpb.vp();
    vp.insert(vp.end(), rpb.vp().begin(), rpb.vp().end());
    BPageB + 1uf buf{ve, vp};
    ppgb.emerge(i, lp.idx());
    // FIXME: Garbage collect @rp
#endif
}

void BTree::_for_each(int p, vector<BNode> &mem, int lv, function<void(BElement)> f) {
    if (p == NIL) {
        return;
    }

    BNode &nd = mem[lv];
    _stg.read_page(nd, p);

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
    for (int i = 0; i <= nd.m; ++i) {
        if (i > 0) {
            int x = nd.e(i).x;
            cout << " (" << x << ") ";
        }
        int pi = nd.p(i);
        if (pi >= 0) {
            cout << pi;
        } else {
            cout << ".";
        }
    }
    cout << "]" << endl;

    for (int i = 0; i <= nd.m; ++i) {
        int pi = nd.p(i);
        if (pi > -1) {
            _dump(pi, lv + 1);
        }
    }
}

void BTree::dump() {
    _dump(hdr.s, 0);
}

BTree::BTree(BTreeStorage &stg) : _stg{stg} {
    hdr = stg.read_header();
    _resize_mem();
}

void BTree::_shrink(BNode &rnd) {
    assert(rnd.m == 0);
    int p0 = rnd.p(0);

    rnd.data.front() = Ep{BElement{}, NIL};

    --hdr.h;
    hdr.s = p0;

    _stg.write_header(hdr);
}

void BTree::_resize_mem() {
    _mem.resize((unsigned long) (hdr.h + 1));
}

//
// Created by kuba on 09.12.16.
//

#ifndef P2_INDEXFILE_H
#define P2_INDEXFILE_H

#include <iostream>
#include "common.h"
#include "PagedFile.h"

enum CompensateStatus {
    COMPENSATE_OK,
    COMPENSATE_NOT_POSSIBLE
};

const int D = 2;

class BMem {

};

class BCache {

};

class BTree {
    PagedFile pgf;
    int s = NIL; // FIXME: Writeback
    int d = D; // FIXME: Dynamic
//    vector<BPage> mem;

    /**
     * @param p initial page index
     * @param x key
     * @return (page, value) or (page, NOT_FOUND)
     */
    pair<int, int> _find(int p, int x);

    void _insert(int p, BElement e);

    CompensateStatus _compensate(int p);

    void _split(int p);

    void _for_each(int p, function<void(pair<int, int>)> f);

    bool page_overflows(BPage &pg);

    bool page_underflows(BPage &pg);

    BPage &make_root(BPageBuf buf);

    void _fix_leaf(BPage &pg);

public:
    BTree(string path);

    /**
     * Find element
     * @param x key
     * @return value or NOT_FOUND
     */
    int find(int x);

    /**
     * Insert element
     * @param x key
     * @param a value
     */
    InsertStatus insert(int x, int a);

    /**
     * Remove element
     * @param x key
     */
    void remove(int x);

    void for_each(function<void(pair<int, int>)> f);

    void _dump(int p);

    void dump();

    void _merge(BPage &lp, BPage &rp, BPageBuf &ppgb, int i);
};

#endif //P2_INDEXFILE_H

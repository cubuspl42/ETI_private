//
// Created by kuba on 09.12.16.
//

#ifndef P2_INDEXFILE_H
#define P2_INDEXFILE_H

#include <iostream>
#include <stack>
#include <vector>
#include "common.h"
#include "BTreeStorage.h"
#include "BNode.h"
#include "BFindResult.h"
#include "Metrics.h"

enum CompensateStatus {
    COMPENSATE_OK,
    COMPENSATE_NOT_POSSIBLE
};


class BTree {
    BTreeStorage &_stg;
    vector<BNode> _mem;
    BTreeHeader hdr;

    BNode &_extra_buf();

    void _allocate_node(BNode &nd, Metrics *m);

    void _free_node(BNode &nd, Metrics *m);

    /**
     * @param p initial page index
     * @param x key
     * @return (page, value) or (page, NOT_FOUND)
     */
    BFindResult _find(int lv, int p, int x, Metrics *m);

    BFindResult _find_max(int lv, int p, Metrics *m);

    CompensateStatus _compensate(BNode &lnd, BNode &pnd, Metrics *m);

    BElement _split(BNode &nd, BNode &nnd);

    void _merge(BNode &nd, BNode &nnd, BNode &pnd, int ei, Metrics *m);

    void _for_each(int p, int lv, function<void(BElement)> f, Metrics *m);

    void _fix_overflow(int lv, Metrics *m);

    void _fix_underflow(int lv, Metrics *m);

    void _grow(int p0, BElement e1, int p1, Metrics *m);

    void _shrink(BNode &rnd, Metrics *m);

    void _resize_mem();

    void _dump(int p, int i, Metrics *m);

public:
    BTree(BTreeStorage &stg);

    /**
     * Find element
     * @param x key
     * @return value or NOT_FOUND
     */
    int find(int x, Metrics *m);

    /**
     * Insert element
     * @param x key
     * @param a value
     */
    InsertStatus insert(int x, int a, Metrics *m);

    /**
     * Remove element
     * @param x key
     * @return value or NOT_FOUND
     */
    int remove(int x, Metrics *m);

    /**
     * Update element
     * @param x key
     * @param na new value
     * @return old value
     */
    int update(int x, int na, Metrics *m);

    void for_each(function<void(BElement)> f, Metrics *m);

    void dump();
};

#endif //P2_INDEXFILE_H

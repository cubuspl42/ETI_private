//
// Created by kuba on 09.12.16.
//

#ifndef P2_INDEXFILE_H
#define P2_INDEXFILE_H

#include <iostream>
#include <stack>
#include <vector>
#include "common.h"
#include "PagedFile.h"
#include "BStorage.h"
#include "BNode.h"
#include "BFindResult.h"

enum CompensateStatus {
    COMPENSATE_OK,
    COMPENSATE_NOT_POSSIBLE
};


class BTree {
    BStorage &_stg;
    vector<BNode> _mem;
    BTreeHeader hdr;
//    int s = NIL; // FIXME: Writeback
    int d = D; // FIXME: Dynamic

    /**
     * @param p initial page index
     * @param x key
     * @return (page, value) or (page, NOT_FOUND)
     */
    BFindResult _find(BStorage &stg, vector<BNode> &mem, int lv, int p, int x);

    BFindResult _find_max(BStorage &stg, vector<BNode> &mem, int lv, int p);

    CompensateStatus _compensate(BStorage &stg, BNode &lnd, BNode &pnd, BNode &rnd);

    BElement _split(BNode &nd, BNode &nnd);

    void _merge(BNode &nd, BNode &nnd, int i);

    void _for_each(int p, vector<BNode> &mem, int lv, function<void(BElement)> f);

    void _fix_overflow(BStorage &stg, std::vector<BNode> &mem, int lv);

    void _fix_underflow(BStorage &stg, std::vector<BNode> &mem, int lv);

public:
    BTree(BStorage &stg);

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

    void for_each(function<void(BElement)> f);

    void _dump(int p, int i);

    void dump();

//    void _merge(BNode &lp, BNode &rp, BPageBuf &ppgb, int i);
};

#endif //P2_INDEXFILE_H

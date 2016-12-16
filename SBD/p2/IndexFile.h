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

/**
 * BIndex ?
 */
class IndexFile {
    PagedFile pgf;
    int s = NIL;
    int d = D; // FIXME

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

    BPage &make_root(BPageBuf buf);

public:
    IndexFile(string path);

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
};

#endif //P2_INDEXFILE_H

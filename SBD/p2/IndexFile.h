//
// Created by kuba on 09.12.16.
//

#ifndef P2_INDEXFILE_H
#define P2_INDEXFILE_H

#include "common.h"
#include "PagedFile.h"

enum CompensateStatus {
    COMPENSATE_OK,
    COMPENSATE_NOT_POSSIBLE
};

/**
 * BIndex ?
 */
template<typename K, typename V>
class IndexFile {
    PagedFile pgf;
    int s = NIL;
    int d = 0; // FIXME

    /**
     * @param p initial page index
     * @param x key
     * @return (page, value) or (page, NOT_FOUND)
     */
    pair<int, int> _find(int p, int x);

    InsertStatus _insert(int p, int x, int a);

    CompensateStatus _compensate(int p);

    void _split(int p);

public:

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
};

#endif //P2_INDEXFILE_H

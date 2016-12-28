//
// Created by kuba on 28.12.16.
//

#ifndef P2_BFINDRESULT_H
#define P2_BFINDRESULT_H

#include "BElement.h"

struct BFindResult {
    /**
     * Page index
     */
    int p;

    /**
     * Value for searched key
     */
    BElement e;

    /**
     * Page level
     */
    int lv;

    /**
     * C-index of the value in the page
     */
    int c;

    BFindResult() = delete;

    BFindResult(int p, BElement e, int lv, int c)
            : p(p), e(e), lv(lv), c(c) {}
};

#endif //P2_BFINDRESULT_H

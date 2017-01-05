//
// Created by kuba on 17.12.16.
//

#ifndef P2_BSTORAGE_H
#define P2_BSTORAGE_H

#include "BNode.h"
#include "BTreeHeader.h"
#include "Metrics.h"

class BTreeStorage {
public:
    virtual BTreeHeader read_header(Metrics *m) = 0;
    virtual void write_header(BTreeHeader header, Metrics *m) = 0;
    virtual void read_page(BNode &pg, int i, Metrics *m, string msg) = 0;

    virtual void write_page(const BNode &pg, Metrics *m, string msg) final {
        write_page(pg, pg.idx, m, msg);
    }

    virtual void write_page(const BNode &pg, int i, Metrics *m, string msg) = 0;

    void dump();
};

#endif //P2_BSTORAGE_H

//
// Created by kuba on 17.12.16.
//

#ifndef P2_BSTORAGE_H
#define P2_BSTORAGE_H

#include "BNode.h"
#include "BTreeHeader.h"

class BStorage {
public:
    virtual BTreeHeader read_header() = 0;
    virtual void write_header(BTreeHeader header) = 0;
    virtual void read_page(BNode &pg, int i) = 0;
    virtual void write_page(const BNode &pg) = 0;
};

#include "PagedFile.h"

#endif //P2_BSTORAGE_H

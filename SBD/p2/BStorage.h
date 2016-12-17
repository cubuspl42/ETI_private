//
// Created by kuba on 17.12.16.
//

#ifndef P2_BSTORAGE_H
#define P2_BSTORAGE_H

#include "BPage.h"

class BStorage {
    void read_page(BPage &pg, int i);
    void write_page(const BPage &pg);
};

#include "PagedFile.h"

#endif //P2_BSTORAGE_H

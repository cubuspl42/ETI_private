//
// Created by kuba on 18.12.16.
//

#include "MemStorage.h"

BTreeHeader MemStorage::read_header() {
    return _hdr;
}

void MemStorage::write_header(BTreeHeader header) {
    _hdr = header;
}

void MemStorage::read_page(BNode &pg, int i) {
    assert(i >= 0);
    assert(i < (int) _nodes.size());
    pg = _nodes[i];
}

void MemStorage::write_page(const BNode &pg) {
    int i = pg.idx;
    assert(i >= 0);
    if(i >= (int) _nodes.size()) {
        _nodes.resize((unsigned long) (i + 1));
    }
    _nodes[i] = pg;
}

//
// Created by kuba on 18.12.16.
//

#include "MemStorage.h"

#include <iostream>

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

bool MemStorage::operator==(const MemStorage &o) {
    return _hdr == o._hdr && _nodes == o._nodes;
}

bool MemStorage::operator!=(const MemStorage &o) {
    return !(*this == o);
}

void MemStorage::dump() {
    cout << "s: " << _hdr.s << " h: " << _hdr.h << " n: " << _hdr.n << endl;
    for(size_t i = 0; i < _nodes.size(); ++i) {
        cout << i << ": ";
        const BNode &nd = _nodes[i];
        nd.dump();
        cout << endl;
    }
}

//
// Created by kuba on 18.12.16.
//

#include "MemStorage.h"
#include "BTreeStorage.h"
#include "Metrics.h"

BTreeHeader MemStorage::read_header(Metrics *m) {
    return _hdr;
}

void MemStorage::write_header(BTreeHeader header, Metrics *m) {
    _hdr = header;
}

void MemStorage::read_page(BNode &pg, int i, Metrics *m, string msg) {
    assert(i >= 0);
    assert(i < (int) _nodes.size());
    pg = _nodes[i];
}

void MemStorage::write_page(const BNode &pg, int i, Metrics *m, string msg) {
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

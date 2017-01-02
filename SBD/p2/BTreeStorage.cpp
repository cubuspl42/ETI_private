//
// Created by kuba on 17.12.16.
//

#include "MemStorage.h"
#include "BTreeStorage.h"

void BTreeStorage::read_page(BNode &pg, int i) {

}

void BTreeStorage::write_page(const BNode &pg) {

}

void BTreeStorage::dump() {
    BTreeHeader hdr = read_header();
    cout << "s: " << hdr.s << " h: " << hdr.h << " n: " << hdr.n << endl;
    for(int i = 0; i < hdr.n; ++i) {
        cout << i << ": ";
        BNode nd;
        read_page(nd, i);
        nd.dump();
        cout << endl;
    }
}
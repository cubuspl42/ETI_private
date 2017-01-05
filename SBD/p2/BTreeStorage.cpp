//
// Created by kuba on 17.12.16.
//

#include "MemStorage.h"
#include "BTreeStorage.h"

void BTreeStorage::dump() {
    BTreeHeader hdr = read_header(nullptr);
    cout << "s: " << hdr.s << " h: " << hdr.h << " n: " << hdr.n << " f: " << hdr.f << endl;
    for(int i = 0; i < hdr.n; ++i) {
        cout << i << ": ";
        BNode nd;
        read_page(nd, i, nullptr);
        nd.dump();
        cout << endl;
    }
}

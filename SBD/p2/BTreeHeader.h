//
// Created by kuba on 17.12.16.
//

#ifndef P2_BTREEHEADER_H
#define P2_BTREEHEADER_H

struct BTreeHeader {
    int s = NIL; // root
    int h = 0; // tree height
    int n = 0; // node count
    int f = NIL; // freelist head

    bool operator==(const BTreeHeader &o) {
        return s == o.s && h == o.h && n == o.n && f == o.f;
    }
};

#endif //P2_BTREEHEADER_H

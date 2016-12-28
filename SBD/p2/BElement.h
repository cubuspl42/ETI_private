//
// Created by kuba on 28.12.16.
//

#ifndef P2_BELEMENT_H
#define P2_BELEMENT_H

struct BElement {
    int x = -1; // key
    int a = -1; // value

    BElement() = default;

    BElement(int _x, int _a) : x(_x), a(_a) {}
};

inline bool operator<(BElement e1, BElement e2) {
    return make_pair(e1.x, e1.a) < make_pair(e2.x, e2.a);
}

inline bool operator==(BElement e1, BElement e2) {
    return make_pair(e1.x, e1.a) == make_pair(e2.x, e2.a);
}

#endif //P2_BELEMENT_H

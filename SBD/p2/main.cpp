#include "PagedFile.h"
#include "IndexedFile.h"
#include "MemStorage.h"

#include <set>

vector<BElement> set2vec(const set<BElement> &s) {
    vector<BElement> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    return v;
}

vector<BElement> bt2vec(BTree &bt) {
    vector<BElement> v;
    bt.for_each([&](BElement e) {
        v.push_back(e);
    });
    return v;
}

static void dump_vec(const vector<BElement> &v) {
    cout << "[ ";
    for(BElement e : v) {
        cout << e.x << " ";
    }
    cout << "]";
}

static void insert(set<BElement> &s, BTree &bt, BElement e) {
    s.insert(e);
    bt.insert(e.x, e.a);
}

static void check(const set<BElement> &s, BTree &bt) {
    auto sv = set2vec(s);
    auto btv = bt2vec(bt);
    if(sv != btv) {
        cout << "sv: ";
        dump_vec(sv);
        cout << endl << endl;

        cout << "bt:" << endl;
        bt.dump();
        cout << endl;

        cout << "btv: ";
        dump_vec(btv);
        cout << endl << endl;
    }
    assert(sv == btv);
}

static bool check_ms(MemStorage ms, MemStorage ms2) {
    if(ms != ms2) {
//        BTree bt{ms};
//        BTree bt2{ms2};
        cout << ">>>>" << endl;
//        bt.dump();
        ms.dump();
        cout << "----" << endl;
//        bt2.dump();
        ms2.dump();
        cout << "<<<<" << endl;
    }
    return ms == ms2;
}

static BNode node(
        int idx,
        int m,
        int p0,
        BElement e1 = BElement{}, int p1 = NIL,
        BElement e2 = BElement{}, int p2 = NIL,
        BElement e3 = BElement{}, int p3 = NIL,
        BElement e4 = BElement{}, int p4 = NIL) {
    BNode nd;
    nd.idx = idx;
    nd.m = m;
    auto &d = nd.data;
    d[0].p = p0;
    d[1].e = e1, d[1].p = p1;
    d[2].e = e2, d[2].p = p2;
    d[3].e = e3, d[3].p = p3;
    d[4].e = e4, d[4].p = p4;
    return nd;
}

static void test_new_root() {
    set<BElement> s;
    MemStorage ms;
    BTree bt{ms};
    insert(s, bt, {1, 2});
    check(s, bt);
}

static void test_insert_simple() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 3, NIL, {1, 0}, NIL, {2, 0}, NIL, {4, 0}, NIL),
                    node(2, 3, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 3, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(3, 0);
    assert(check_ms(ms, ms2));
}

static void test_split_root() {
    MemStorage ms{
            //s  h  n
            {0, 1, 1},
            {
                    node(0, 4, NIL, {1, 2}, NIL, {2, 3}, NIL, {3, 4}, NIL, {4, 5}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {2, 2, 3},
            {
                    node(0, 2, NIL, {1, 2}, NIL, {2, 3}, NIL),
                    node(1, 2, NIL, {4, 5}, NIL, {5, 6}, NIL),
                    node(2, 1, 0, {3, 4}, 1),
            }
    };
    BTree bt{ms};
    bt.insert(5, 6);
    assert(check_ms(ms, ms2));
}

static void test_split_chained() {
    MemStorage ms{
            //s  h  n
            {0, 2, 6},
            {
                    node(0, 4, 1, {10, 0}, 2, {20, 0}, 3, {30, 0}, 4, {40, 0}, 5),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node(3, 4, NIL, {21, 0}, NIL, {22, 0}, NIL, {24, 0}, NIL, {25, 0}, NIL),
                    node(4, 4, NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node(5, 4, NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {8, 3, 9},
            {
                    node(0, 2, 1, {10, 0}, 2, {20, 0}, 3),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node(3, 2, NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node(4, 4, NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node(5, 4, NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node(6, 2, NIL, {24, 0}, NIL, {25, 0}, NIL),
                    node(7, 2, 6, {30, 0}, 4, {40, 0}, 5),
                    node(8, 1, 0, {23, 0}, 7),
            }
    };
    BTree bt{ms};
    bt.insert(23, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_even() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 2, NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {5, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 3, NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(5, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_odd() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {4, 0}, NIL, {5, 0}, NIL),
                    node(2, 3, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {5, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 4, NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(3, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_inner_node() {
    MemStorage ms{
            //s  h  n
            {0, 3, 11},
            {
                    node(0, 1, 1, {100, 0}, 2),
                    node(1, 4, 3, {10, 0}, 4, {20, 0}, 5, {30, 0}, 6, {40, 0}, 7),
                    node(2, 2, 8, {210, 0}, 9, {220, 0}, 10),
                    node(3, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(4, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node(5, 4, NIL, {21, 0}, NIL, {22, 0}, NIL, {23, 0}, NIL, {24, 0}, NIL),
                    node(6, 4, NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node(7, 4, NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node(8, 4, NIL, {201, 0}, NIL, {202, 0}, NIL, {203, 0}, NIL, {204, 0}, NIL),
                    node(9, 4, NIL, {211, 0}, NIL, {212, 0}, NIL, {213, 0}, NIL, {214, 0}, NIL),
                    node(10, 4, NIL, {221, 0}, NIL, {222, 0}, NIL, {223, 0}, NIL, {224, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 3, 12},
            {
                    node(0, 1, 1, {40, 0}, 2),
                    node(1, 4, 3, {10, 0}, 4, {20, 0}, 5, {23, 0}, 11, {30, 0}, 6),
                    node(2, 3, 7, {100, 0}, 8, {210, 0}, 9, {220, 0}, 10),
                    node(3, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(4, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node(5, 2, NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node(6, 4, NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node(7, 4, NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node(8, 4, NIL, {201, 0}, NIL, {202, 0}, NIL, {203, 0}, NIL, {204, 0}, NIL),
                    node(9, 4, NIL, {211, 0}, NIL, {212, 0}, NIL, {213, 0}, NIL, {214, 0}, NIL),
                    node(10, 4, NIL, {221, 0}, NIL, {222, 0}, NIL, {223, 0}, NIL, {224, 0}, NIL),
                    node(11, 2, NIL, {24, 0}, NIL, {25, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(25, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_left() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 3, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node(2, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {14, 0}, NIL, {15, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {11, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {10, 0}, NIL),
                    node(2, 4, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL, {15, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(13, 0);
    assert(check_ms(ms, ms2));
}

static void test_remove_compensate_left() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 3, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node(2, 2, NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {3, 0}, 2),
                    node(1, 2, NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node(2, 2, NIL, {10, 0}, NIL, {11, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(12);
    assert(check_ms(ms, ms2));
}

static void test_remove_compensate_right() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 2, NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node(2, 3, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {11, 0}, 2),
                    node(1, 2, NIL, {1, 0}, NIL, {10, 0}, NIL),
                    node(2, 2, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(2);
    assert(check_ms(ms, ms2));
}

static void test_remove_merge() {
    MemStorage ms{
            //s  h  n
            {0, 2, 5},
            {
                    node(0, 3, 1, {10, 0}, 2, {20, 0}, 3, {30, 0}, 4),
                    node(1, 2, NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node(2, 2, NIL, {11, 0}, NIL, {12, 0}, NIL),
                    node(3, 2, NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node(4, 2, NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 5},
            {
                    node(0, 2, 1, {20, 0}, 3, {30, 0}, 4),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {10, 0}, NIL, {11, 0}, NIL),
                    node(2, 0, NIL),
                    node(3, 2, NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node(4, 2, NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(12);
    assert(check_ms(ms, ms2));
}

static void test_remove_merge_root() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 2, NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node(2, 2, NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {1, 1, 3},
            {
                    node(0, 0, NIL),
                    node(1, 4, NIL, {1, 0}, NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL),
                    node(2, 0, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(2);
    assert(check_ms(ms, ms2));
}


static void test_remove_swap() {
    MemStorage ms{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {10, 0}, 2),
                    node(1, 4, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node(2, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
            }
    };
    const MemStorage ms2{
            //s  h  n
            {0, 2, 3},
            {
                    node(0, 1, 1, {4, 0}, 2),
                    node(1, 3, NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node(2, 4, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(10);
    assert(check_ms(ms, ms2));
}

int main(int argc, const char *argv[]) {
    test_new_root();
    test_insert_simple();
    test_split_root();
    test_split_chained();
    test_compensate_even();
    test_compensate_odd();
    test_compensate_inner_node();
    test_compensate_left();
    test_remove_compensate_left();
    test_remove_compensate_right();
    test_remove_merge();
    test_remove_merge_root();
    test_remove_swap();
    return 0;
}

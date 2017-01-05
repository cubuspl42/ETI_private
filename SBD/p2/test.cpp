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

static bool check_ms(MemStorage ac, MemStorage ex) {
    if(ac != ex) {
//        BTree bt{ms};
//        BTree bt2{ms2};
        cout << ">>>> actual" << endl;
//        bt.dump();
        ac.dump();
        cout << "----" << endl;
//        bt2.dump();
        ex.dump();
        cout << "<<<< expected" << endl;
    }
    return ac == ex;
}

static bool check_ms2(MemStorage ac, MemStorage ex, BTree &bt) {
    if(ac != ex) {
        cout << "**** bt" << endl;
        bt.dump();
        cout << ">>>> actual" << endl;
        ac.dump();
        cout << "----" << endl;
        ex.dump();
        cout << "<<<< expected" << endl;
    }
    return ac == ex;
}

static BNode nodeD(int next) {
    BNode nd;
    nd.idx = next;
    nd.m = 0;
    auto &d = nd.data;
    d[0].p = NIL;
    d[1].e = BElement{};
    return nd;
}

static BNode node1(int p0, BElement e1, int p1) {
    BNode nd;
    nd.m = 1;
    auto &d = nd.data;
    d[0].p = p0;
    d[1].e = e1, d[1].p = p1;
    return nd;
}

static BNode node2(int p0, BElement e1, int p1, BElement e2, int p2) {
    BNode nd;
    nd.m = 2;
    auto &d = nd.data;
    d[0].p = p0;
    d[1].e = e1, d[1].p = p1;
    d[2].e = e2, d[2].p = p2;
    return nd;
}

static BNode node3(int p0, BElement e1, int p1, BElement e2, int p2, BElement e3, int p3) {
    BNode nd;
    nd.m = 3;
    auto &d = nd.data;
    d[0].p = p0;
    d[1].e = e1, d[1].p = p1;
    d[2].e = e2, d[2].p = p2;
    d[3].e = e3, d[3].p = p3;
    return nd;
}

static BNode node4(int p0, BElement e1, int p1, BElement e2, int p2, BElement e3, int p3, BElement e4, int p4) {
    BNode nd;
    nd.m = 4;
    auto &d = nd.data;
    d[0].p = p0;
    d[1].e = e1, d[1].p = p1;
    d[2].e = e2, d[2].p = p2;
    d[3].e = e3, d[3].p = p3;
    d[4].e = e4, d[4].p = p4;
    return nd;
}

static void test_find_simple() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 123}, NIL, {4, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    int a = bt.find(2);
    assert(a == 123);
}


static void test_find_not_found() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 123}, NIL, {4, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {14, 0}, NIL),
            }
    };
    BTree bt{ms};
    int a = bt.find(13);
    assert(a == NOT_FOUND);
}

static void test_find_empty_tree() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    BTree bt{ms};
    int a = bt.find(13);
    assert(a == NOT_FOUND);
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
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 0}, NIL, {4, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(3, 0);
    assert(check_ms(ms, ms2));
}

static void test_insert2() {
    MemStorage ms{
    //       s  h  n  f
            {0, 1, 1, NIL},
            {
                    node1(NIL, {10, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 1, 1, NIL},
            {
                    node3(NIL, {10, 0}, NIL, {20, 0}, NIL, {30, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(20, 0);
    bt.insert(30, 0);
    assert(check_ms(ms, ms2));
}

static void test_split_root() {
    MemStorage ms{
    //       s  h  n  f
            {0, 1, 1, NIL},
            {
                    node4(NIL, {1, 2}, NIL, {2, 3}, NIL, {3, 4}, NIL, {4, 5}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 3, NIL},
            {
                    node2(NIL, {1, 2}, NIL, {2, 3}, NIL),
                    node2(NIL, {4, 5}, NIL, {5, 6}, NIL),
                    node1(0, {3, 4}, 1),
            }
    };
    BTree bt{ms};
    bt.insert(5, 6);
    assert(check_ms(ms, ms2));
}

static void test_insert_split_root_reuse() {
    MemStorage ms{
    //       s  h  n  f
            {1, 1, 2, 0},
            {
                    nodeD(NIL),
                    node4(NIL, {1, 2}, NIL, {2, 3}, NIL, {3, 4}, NIL, {4, 5}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 3, NIL},
            {
                    node2(NIL, {4, 5}, NIL, {5, 6}, NIL),
                    node2(NIL, {1, 2}, NIL, {2, 3}, NIL),
                    node1(1, {3, 4}, 0),
            }
    };
    BTree bt{ms};
    bt.insert(5, 6);
    assert(check_ms(ms, ms2));
}

static void test_insert_split_root_reuse2() {
    MemStorage ms{
    //       s  h  n  f
            {2, 1, 3, 0},
            {
                    nodeD(1),
                    nodeD(NIL),
                    node4(NIL, {1, 2}, NIL, {2, 3}, NIL, {3, 4}, NIL, {4, 5}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {1, 2, 3, NIL},
            {
                    node2(NIL, {4, 5}, NIL, {5, 6}, NIL),
                    node1(2, {3, 4}, 0),
                    node2(NIL, {1, 2}, NIL, {2, 3}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(5, 6);
    assert(check_ms(ms, ms2));
}

static void test_insert_full() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 1, 1, NIL},
            {
                    node4(NIL, {10, 0}, NIL, {20, 0}, NIL, {30, 0}, NIL, {40, 0}, NIL)
            }
    };
    BTree bt{ms};
    bt.insert(10, 0);
    bt.insert(20, 0);
    bt.insert(30, 0);
    bt.insert(40, 0);
    assert(check_ms(ms, ms2));
}

static void test_insert_stairs() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 3, NIL},
            {
                    node2(NIL, {10, 0}, NIL, {11, 0}, NIL),
                    node2(NIL, {30, 0}, NIL, {40, 0}, NIL),
                    node1(0, {20, 0}, 1),
            }
    };
    BTree bt{ms};
    bt.insert(10, 0);
    bt.insert(20, 0);
    bt.insert(30, 0);
    bt.insert(40, 0);
    bt.insert(11, 0);
    assert(check_ms(ms, ms2));
}

static void test_insert_stairs2() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 3, NIL},
            {
                    node4(NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
                    node4(NIL, {20, 0}, NIL, {21, 0}, NIL, {30, 0}, NIL, {40, 0}, NIL),
                    node1(0, {14, 0}, 1),
            }
    };
    BTree bt{ms};
    bt.insert(10, 0);
    bt.insert(20, 0);
    bt.insert(30, 0);
    bt.insert(40, 0);
    bt.insert(11, 0);
    bt.insert(12, 0);
    bt.insert(13, 0);
    bt.insert(14, 0);
    bt.insert(21, 0);
    assert(check_ms(ms, ms2));
}

static void test_insert_stairs3() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 4, NIL},
            {
                    node4(NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
                    node2(NIL, {20, 0}, NIL, {21, 0}, NIL),
                    node2(0, {14, 0}, 1, {22, 0}, 3),
                    node2(NIL, {30, 0}, NIL, {40, 0}, NIL)
            }
    };
    BTree bt{ms};
    bt.insert(10, 0);
    bt.insert(20, 0);
    bt.insert(30, 0);
    bt.insert(40, 0);
    bt.insert(11, 0);
    bt.insert(12, 0);
    bt.insert(13, 0);
    bt.insert(14, 0);
    bt.insert(21, 0);
    bt.insert(22, 0);
    assert(check_ms(ms, ms2));
}

static void test_insert_stairs4() {
    MemStorage ms{
    //       s  h  n  f
            {NIL, 0, 0, NIL},
            {}
    };
    const MemStorage ms2{
    //       s  h  n  f
            {2, 2, 4, NIL},
            {
                    node4(NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
                    node4(NIL, {20, 0}, NIL, {21, 0}, NIL, {22, 0}, NIL, {23, 0}, NIL),
                    node2(0, {14, 0}, 1, {24, 0}, 3),
                    node3(NIL, {30, 0}, NIL, {31, 0}, NIL, {40, 0}, NIL)
            }
    };
    BTree bt{ms};
    bt.insert(10, 0);
    bt.insert(20, 0);
    bt.insert(30, 0);
    bt.insert(40, 0);
    bt.insert(11, 0);
    bt.insert(12, 0);
    bt.insert(13, 0);
    bt.insert(14, 0);
    bt.insert(21, 0);
    bt.insert(22, 0);
    bt.insert(23, 0);
    bt.insert(24, 0);
    bt.insert(31, 0);
    // bt.insert(32, 0);
    // bt.insert(33, 0);
    // bt.insert(34, 0);
    // bt.insert(25, 0);
    assert(check_ms(ms, ms2));
}

static void test_split_chained() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 6, NIL},
            {
                    node4(1, {10, 0}, 2, {20, 0}, 3, {30, 0}, 4, {40, 0}, 5),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node4(NIL, {21, 0}, NIL, {22, 0}, NIL, {24, 0}, NIL, {25, 0}, NIL),
                    node4(NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node4(NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {8, 3, 9, NIL},
            {
                    node2(1, {10, 0}, 2, {20, 0}, 3),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node4(NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node4(NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node2(NIL, {24, 0}, NIL, {25, 0}, NIL),
                    node2(6, {30, 0}, 4, {40, 0}, 5),
                    node1(0, {23, 0}, 7),
            }
    };
    BTree bt{ms};
    bt.insert(23, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_even() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node2(NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {5, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node3(NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(5, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_odd() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {4, 0}, NIL, {5, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {5, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(3, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_inner_node() {
    MemStorage ms{
    //       s  h  n  f
            {0, 3, 11, NIL},
            {
                    node1(1, {100, 0}, 2),
                    node4(3, {10, 0}, 4, {20, 0}, 5, {30, 0}, 6, {40, 0}, 7),
                    node2(8, {210, 0}, 9, {220, 0}, 10),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node4(NIL, {21, 0}, NIL, {22, 0}, NIL, {23, 0}, NIL, {24, 0}, NIL),
                    node4(NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node4(NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node4(NIL, {201, 0}, NIL, {202, 0}, NIL, {203, 0}, NIL, {204, 0}, NIL),
                    node4(NIL, {211, 0}, NIL, {212, 0}, NIL, {213, 0}, NIL, {214, 0}, NIL),
                    node4(NIL, {221, 0}, NIL, {222, 0}, NIL, {223, 0}, NIL, {224, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 3, 12, NIL},
            {
                    node1(1, {40, 0}, 2),
                    node4(3, {10, 0}, 4, {20, 0}, 5, {23, 0}, 11, {30, 0}, 6),
                    node3(7, {100, 0}, 8, {210, 0}, 9, {220, 0}, 10),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node4(NIL, {31, 0}, NIL, {32, 0}, NIL, {33, 0}, NIL, {34, 0}, NIL),
                    node4(NIL, {41, 0}, NIL, {42, 0}, NIL, {43, 0}, NIL, {44, 0}, NIL),
                    node4(NIL, {201, 0}, NIL, {202, 0}, NIL, {203, 0}, NIL, {204, 0}, NIL),
                    node4(NIL, {211, 0}, NIL, {212, 0}, NIL, {213, 0}, NIL, {214, 0}, NIL),
                    node4(NIL, {221, 0}, NIL, {222, 0}, NIL, {223, 0}, NIL, {224, 0}, NIL),
                    node2(NIL, {24, 0}, NIL, {25, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(25, 0);
    assert(check_ms(ms, ms2));
}

static void test_compensate_left() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {14, 0}, NIL, {15, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {11, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {10, 0}, NIL),
                    node4(NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL, {15, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.insert(13, 0);
    assert(check_ms(ms, ms2));
}

static void test_remove_compensate_left() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node2(NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {3, 0}, 2),
                    node2(NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node2(NIL, {10, 0}, NIL, {11, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(12);
    assert(check_ms(ms, ms2));
}

static void test_remove_compensate_right() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node2(NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node3(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {11, 0}, 2),
                    node2(NIL, {1, 0}, NIL, {10, 0}, NIL),
                    node2(NIL, {12, 0}, NIL, {13, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(2);
    assert(check_ms(ms, ms2));
}

static void test_remove_merge() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 5, NIL},
            {
                    node3(1, {10, 0}, 2, {20, 0}, 3, {30, 0}, 4),
                    node2(NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node2(NIL, {11, 0}, NIL, {12, 0}, NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node2(NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 5, NIL},
            {
                    node2(1, {20, 0}, 3, {30, 0}, 4),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {10, 0}, NIL, {11, 0}, NIL),
                    nodeD(NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node2(NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(12);
    assert(check_ms(ms, ms2));
}

static void test_remove_merge_p0() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 5, NIL},
            {
                    node3(1, {10, 0}, 2, {20, 0}, 3, {30, 0}, 4),
                    node2(NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node2(NIL, {11, 0}, NIL, {12, 0}, NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node2(NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 5, NIL},
            {
                    node2(1, {20, 0}, 3, {30, 0}, 4),
                    node4(NIL, {1, 0}, NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL),
                    nodeD(NIL),
                    node2(NIL, {21, 0}, NIL, {22, 0}, NIL),
                    node2(NIL, {31, 0}, NIL, {32, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(2);
    assert(check_ms(ms, ms2));
}


static void test_remove_merge_shrink() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node2(NIL, {1, 0}, NIL, {2, 0}, NIL),
                    node2(NIL, {11, 0}, NIL, {12, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {1, 1, 3, 0},
            {
                    nodeD(2),
                    node4(NIL, {1, 0}, NIL, {10, 0}, NIL, {11, 0}, NIL, {12, 0}, NIL),
                    nodeD(NIL),
            }
    };
    BTree bt{ms};
    bt.remove(2);
    assert(check_ms(ms, ms2));
}

static void test_remove_swap() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {4, 0}, 2),
                    node3(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 0}, NIL, {14, 0}, NIL),
            }
    };
    BTree bt{ms};
    bt.remove(10);
    assert(check_ms2(ms, ms2, bt));
}

static void test_remove_root() {
    MemStorage ms{
    //       s  h  n  f
            {0, 1, 1, NIL},
            {
                    node1(NIL, {10, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s    h  n  f
            {NIL, 0, 1, 0},
            {
                    nodeD(-1),
            }
    };
    BTree bt{ms};
    bt.remove(10);
    assert(check_ms2(ms, ms2, bt));
}

static void test_update_simple() {
    MemStorage ms{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 123}, NIL, {14, 0}, NIL),
            }
    };
    const MemStorage ms2{
    //       s  h  n  f
            {0, 2, 3, NIL},
            {
                    node1(1, {10, 0}, 2),
                    node4(NIL, {1, 0}, NIL, {2, 0}, NIL, {3, 0}, NIL, {4, 0}, NIL),
                    node4(NIL, {11, 0}, NIL, {12, 0}, NIL, {13, 124}, NIL, {14, 0}, NIL),
            }
    };
    BTree bt{ms};
    assert(bt.update(13, 124) == 123);
    assert(check_ms2(ms, ms2, bt));
}

int main(int argc, const char *argv[]) {
    test_find_simple();
    test_find_not_found();
    test_find_empty_tree();
    test_new_root();
    test_insert_simple();
    test_insert2();
    test_insert_full();
    test_insert_stairs();
    test_insert_stairs2();
    test_insert_stairs3();
    test_insert_stairs4();
    test_split_root();
    test_insert_split_root_reuse();
    test_insert_split_root_reuse2();
    test_split_chained();
    test_compensate_even();
    test_compensate_odd();
    test_compensate_inner_node();
    test_compensate_left();
    test_remove_compensate_left();
    test_remove_compensate_right();
    test_remove_merge();
    test_remove_merge_p0();
    test_remove_merge_shrink();
    test_remove_swap();
    test_remove_root();
    test_update_simple();
    return 0;
}

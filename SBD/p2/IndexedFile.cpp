#include "IndexedFile.h"

#if 0
IndexedFile::IndexedFile(string path)
        : cf{path} {
}

void IndexedFile::insert(Record r) {
    int x = (int) r.pkey(); // FIXME: i64
    if(bt.find(x) == NOT_FOUND) {
        int a = cf.write_record(r);
        bt.insert(x, a);
    }
}

void IndexedFile::for_each(function<void(Record)> f) {
    bt.for_each([&](pair<int, int> xa) {
        Record r = cf.read_record(xa.second);
        assert(r.pkey() == xa.first);
        f(r);
    });
}

vector<Record> IndexedFile::to_vector() {
    vector<Record> v;
    for_each([&](Record r) {
        v.push_back(r);
    });
    return v;
}

void IndexedFile::dump() {
    bt.dump();
}

IndexedFile tmp_indexed_file() {
    return IndexedFile(tmpnam(nullptr));
}
#endif
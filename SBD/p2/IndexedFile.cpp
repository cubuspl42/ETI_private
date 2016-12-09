#include "IndexedFile.h"

IndexedFile::IndexedFile(string path)
        : ixf{path + "_idx"}, cf{path} {
}

void IndexedFile::insert(Record r) {
    int x = (int) r.pkey(); // FIXME: i64
    if(ixf.find(x) == NOT_FOUND) {
        int a = cf.write_record(r);
        ixf.insert(x, a);
    }
}

void IndexedFile::for_each(function<void(Record)> f) {
    ixf.for_each([&](pair<int, int> xa) {
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

IndexedFile tmp_indexed_file() {
    return IndexedFile(tmpnam(nullptr));
}

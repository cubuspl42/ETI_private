#include "IndexedFile.h"


IndexedFile::IndexedFile(string path)
        : storage{path + "_index"}, index{storage}, content{path} {}

void IndexedFile::insert(Record r) {
    int x = (int) r.pkey();// FIXME: i64
    // FIXME: user-selected key, not eq from p1
    assert(index.find(x) == NOT_FOUND); // FIXME: assert
    int a = content.write_record(r);
    index.insert(x, a);
}

bool IndexedFile::contains(Record r) {
    return index.find((int) r.pkey()) != NOT_FOUND;
}

void IndexedFile::for_each(function<void(Record)> f) {
    // TODO: implement
#if 0
    index.for_each([&](pair<int, int> xa) {
        Record r = content.read_record(xa.second);
        assert(r.pkey() == xa.first);
        f(r);
    });
#endif
}

vector<Record> IndexedFile::to_vector() {
    vector<Record> v;
    for_each([&](Record r) {
        v.push_back(r);
    });
    return v;
}

void IndexedFile::dump() {
    cout << "storage.dump:" << endl;
    storage.dump();
    cout << "index.dump:" << endl;
    index.dump();
}

IndexedFile tmp_indexed_file() {
    return IndexedFile(tmpnam(nullptr));
}

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


void IndexedFile::remove(Record r) {
    int x = (int) r.pkey();
    int a = index.remove(x);
    assert(a != NOT_FOUND);
    // TODO: Release space in content file?
}

void IndexedFile::for_each(function<void(Record)> f) {
    index.for_each([&](BElement e) {
        Record r = content.read_record(e.a);
        assert(r.pkey() == e.x);
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
    cout << "storage.dump:" << endl;
    storage.dump();
    cout << "index.dump:" << endl;
    index.dump();
}

IndexedFile tmp_indexed_file() {
    return IndexedFile(tmpnam(nullptr));
}

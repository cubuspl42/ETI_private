#include "IndexedFile.h"


IndexedFile::IndexedFile(const string &path, string mode)
        : storage{path + "_index", mode}, index{storage}, content{path, mode} {}

void IndexedFile::insert(int k, Record r, Metrics *m) {
    assert(index.find(k, nullptr) == NOT_FOUND);
    int a = content.write_record(r);
    index.insert(k, a, m);
}

pair<bool, Record> IndexedFile::find(int k, Metrics *m) {
    int a = index.find(k, m);
    if(a == NOT_FOUND) {
        return make_pair(false, Record{});
    } else {
        Record r = content.read_record(a);
        return make_pair(true, r);
    }
}

Record IndexedFile::remove(int k, Metrics *m) {
    int a = index.remove(k, m);
    assert(a != NOT_FOUND);
    Record r = content.read_record(a);
    return r;
    // TODO: Release space in content file?
}

Record IndexedFile::update(int k, Record nr, Metrics *m) {
    int a = index.find(k, m);
    assert(a != NOT_FOUND);
    Record olr = content.read_record(a);
    content.write_record(nr, a);
    return olr;
}

void IndexedFile::for_each(function<void(int, Record)> f, Metrics *m) {
    index.for_each([&](BElement e) {
        int k = e.x;
        Record r = content.read_record(e.a);
        f(k, r);
    }, m);
}

vector<pair<int, Record>> IndexedFile::to_vector() {
    vector<pair<int, Record>> v;
    for_each([&](int k, Record r) {
        v.push_back({k, r});
    }, nullptr);
    return v;
}

void IndexedFile::dump() {
    cout << "storage.dump:" << endl;
    storage.dump();
    cout << "index.dump:" << endl;
    index.dump();
}

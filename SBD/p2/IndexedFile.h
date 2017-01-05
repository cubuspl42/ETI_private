#ifndef P2_INDEXEDFILE_H
#define P2_INDEXEDFILE_H

#include "Record.h"
#include "BTree.h"
#include "ContentFile.h"
#include "FileStorage.h"

#include <string>

using namespace std;

class IndexedFile {
    FileStorage storage;
    BTree index;
    ContentFile content;
public:
    IndexedFile() = delete;

    IndexedFile(const string &path, string mode);

    void insert(int k, Record r, Metrics *m);

    pair<bool, Record> find(int k, Metrics *m);

    Record remove(int k, Metrics *m);

    Record update(int k, Record nr, Metrics *m);

    void for_each(function<void(int, Record)> f, Metrics *m);

    vector<pair<int, Record>> to_vector();

    void dump();
};

#endif //P2_INDEXEDFILE_H

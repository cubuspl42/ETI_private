#ifndef P2_INDEXEDFILE_H
#define P2_INDEXEDFILE_H

#include "PagedFile.h"
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

    IndexedFile(const string &path);

    void insert(int k, Record r);

    pair<bool, Record> find(int k);

    Record remove(int k);

    Record update(int k, Record nr);

    void for_each(function<void(int, Record)> f);

    vector<pair<int, Record>> to_vector();

    void dump();
};

#endif //P2_INDEXEDFILE_H

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
    IndexedFile(string path);

    void insert(int k, Record r);

    pair<bool, Record> find(int k);

    Record remove(int k);

    void for_each(function<void(int, Record)> f);

    vector<pair<int, Record>> to_vector();

    void dump();
};

IndexedFile tmp_indexed_file();

#endif //P2_INDEXEDFILE_H

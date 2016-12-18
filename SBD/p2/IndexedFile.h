#ifndef P2_INDEXEDFILE_H
#define P2_INDEXEDFILE_H

#include "PagedFile.h"
#include "Record.h"
#include "BTree.h"
#include "ContentFile.h"

#include <string>

using namespace std;

class IndexedFile {
    BTree bt;
    ContentFile cf;
public:
    IndexedFile() = delete;
    IndexedFile(string path);

    void insert(Record r);

    void for_each(function<void(Record)> f);

    vector<Record> to_vector();

    void dump();
};

IndexedFile tmp_indexed_file();

#endif //P2_INDEXEDFILE_H

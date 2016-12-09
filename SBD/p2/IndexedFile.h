#ifndef P2_INDEXEDFILE_H
#define P2_INDEXEDFILE_H

#include "PagedFile.h"
#include "Record.h"
#include "IndexFile.h"

#include <string>

using namespace std;

class IndexedFile {
    PagedFile pgf;
    IndexFile ixf;
public:
    IndexedFile() = delete;
    IndexedFile(string path);

    void insert(Record r);
};

IndexedFile tmp_indexed_file();

#endif //P2_INDEXEDFILE_H

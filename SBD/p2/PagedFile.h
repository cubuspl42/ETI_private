#ifndef P2_FILE_H
#define P2_FILE_H

#include "BPage.h"

#include <algorithm>
#include <memory>
#include <string>

using namespace std;

class PagedFile {
    FilePtr _file;
    int _pgsz;
    vector<unique_ptr<BPage>> _cache;
    int _next_idx = NIL;

    BPage *_find_page(int idx);
    BPage _read_page(int idx);
public:
    PagedFile(string path, int pgsz);
    BPage &read_page(int idx);
    BPage &make_page(int parent, BPageBuf buf);
};

#endif //P2_FILE_H

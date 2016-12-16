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
    int bpp;

    BPage *_find_page(int idx);
    BPage _read_page(int idx);
    void _write_page(const BPage &pg);
public:
    PagedFile(string path, int pgsz);
    BPage &read_page(int idx);
    BPage &make_page(int parent, BPageBuf buf);
    void write_back();

};

#endif //P2_FILE_H

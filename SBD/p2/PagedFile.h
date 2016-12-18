#ifndef P2_FILE_H
#define P2_FILE_H

#include "BNode.h"

#include <algorithm>
#include <memory>
#include <string>

using namespace std;

class PagedFile {
    FilePtr _file;
    int _pgsz;
    vector<unique_ptr<BNode>> _cache;
    int _next_idx = NIL;
    int bpp;

    BNode *_find_page(int idx);
    BNode _read_page(int idx);
    void _write_page(const BNode &pg);
public:
    PagedFile(string path, int pgsz);
    BNode &read_page(int idx);
    void write_back();

};

#endif //P2_FILE_H

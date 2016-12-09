#include "PagedFile.h"
#include "common.h"
#include "Record.h"

#include <cassert>

PagedFile::PagedFile(string path, int pgsz)
        : _file{fopen(path.c_str(), "wb+"), file_close}, _pgsz(pgsz) {
    int rv = fseek(_file.get(), 0, SEEK_END);
    assert(rv == 0);
    int fsz = (int) ftell(_file.get());
    int bpp = sizeof(BPageHeader) + sizeof(BEntry) * pgsz; // bytes per page
    assert(fsz % bpp == 0);
    int np = fsz / bpp;
    _next_idx = np;
}

BPage *PagedFile::_find_page(int idx) {
    auto it = find_if(_cache.begin(), _cache.end(), [&](const BPage &pg) {
        return pg.idx() == idx;
    });
    if(it != _cache.end()) {
        return &*it;
    } else {
        return nullptr;
    }
}

BPage PagedFile::_read_page(int idx) {
    assert(idx >= 0);
    int rv = fseek(_file.get(), sizeof(Record) * idx , SEEK_SET);
    assert(rv == 0);
    BPageHeader pgh;
    int nr = fread(&pgh, sizeof(BPageHeader), 1, _file.get());
    assert(nr == 1);
    assert(pgh.m <= _pgsz);
    vector<BEntry> v((unsigned long) pgh.m);
    nr = fread(v.data(), sizeof(BEntry), pgh.m, _file.get());
    assert(nr == pgh.m);
    return move(BPage{idx, pgh.parent, v});
}

BPage &PagedFile::read_page(int idx) {
    BPage *cpg = _find_page(idx);
    if(cpg) {
        return *cpg;
    } else {
        _cache.push_back(_read_page(idx));
        return _cache.back();
    }
}

BPage &PagedFile::make_page(int parent, vector<BEntry> data) {
    BPage pg{_next_idx++, parent, move(data)};
    assert(_find_page(pg.idx()) == nullptr);
    _cache.push_back(move(pg));
    return _cache.back();
}

BPage &PagedFile::make_page(int parent) {
    return make_page(parent, {BEntry{}});
}

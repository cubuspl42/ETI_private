#include "PagedFile.h"
#include "common.h"
#include "Record.h"

#include <cassert>

PagedFile::PagedFile(string path, int pgsz)
        : _file{fopen(path.c_str(), "wb+"), file_close}, _pgsz(pgsz) {
    int rv = fseek(_file.get(), 0, SEEK_END);
    assert(rv == 0);
    int fsz = (int) ftell(_file.get());
    int bpp = sizeof(BPageHeader) + sizeof(BElement) * pgsz + sizeof(int) * (pgsz + 1); // bytes per page
    assert(fsz % bpp == 0);
    int np = fsz / bpp;
    _next_idx = np;
}

BPage *PagedFile::_find_page(int idx) {
    auto it = find_if(_cache.begin(), _cache.end(), [&](const unique_ptr<BPage> &pg) {
        return pg.get()->idx() == idx;
    });
    if(it != _cache.end()) {
        BPage *ptr = it->get();
        assert(ptr);
        return ptr;
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
    vector<BElement> e((unsigned long) pgh.m);
    vector<int> p((unsigned long) pgh.m + 1);
    nr = fread(e.data(), sizeof(BElement), pgh.m, _file.get());
    assert(nr == pgh.m);
    nr = fread(p.data(), sizeof(int), pgh.m + 1, _file.get());
    assert(nr == pgh.m + 1);
    return move(BPage{idx, pgh.parent, BPageBuf{e, p}});
}

BPage &PagedFile::read_page(int idx) {
    BPage *cpg = _find_page(idx);
    if(cpg) {
        return *cpg;
    } else {
        _cache.emplace_back(new BPage(_read_page(idx)));
        BPage *ptr = _cache.back().get();
        assert(ptr);
        return *ptr;
    }
}

BPage &PagedFile::make_page(int parent, BPageBuf buf) {
    BPage pg{_next_idx++, parent, move(buf)};
    assert(_find_page(pg.idx()) == nullptr);
    _cache.emplace_back(new BPage(move(pg)));
    BPage *ptr = _cache.back().get();
    assert(ptr);
    return *ptr;
}

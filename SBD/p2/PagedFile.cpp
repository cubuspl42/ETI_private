#include "PagedFile.h"
#include "common.h"
#include "Record.h"

#include <cassert>

#if 0
PagedFile::PagedFile(string path, int pgsz)
        : _file{fopen(path.c_str(), "wb+"), file_close}, _pgsz(pgsz) {
    bpp = sizeof(BPageHeader) + sizeof(BElement) * pgsz + sizeof(int) * (pgsz + 1); // bytes per page
    int rv = fseek(_file.get(), 0, SEEK_END);
    assert(rv == 0);
    int fsz = (int) ftell(_file.get());
    assert(fsz % bpp == 0);
    int np = fsz / bpp;
    _next_idx = np;
}

BNode *PagedFile::_find_page(int idx) {
    auto it = find_if(_cache.begin(), _cache.end(), [&](const unique_ptr<BNode> &pg) {
        return pg.get()->idx() == idx;
    });
    if(it != _cache.end()) {
        BNode *ptr = it->get();
        assert(ptr);
        return ptr;
    } else {
        return nullptr;
    }
}

BNode PagedFile::_read_page(int idx) {
    assert(idx >= 0);
    int rv = fseek(_file.get(), bpp * idx, SEEK_SET);
    assert(rv == 0);
    BPageHeader pgh;
    int nr = fread(&pgh, sizeof(BPageHeader), 1, _file.get());
    assert(nr == 1);
    assert(pgh.m <= _pgsz);
    vector<BElement> e((unsigned long) _pgsz);
    vector<int> p((unsigned long) _pgsz + 1);
    nr = fread(e.data(), sizeof(BElement), _pgsz, _file.get());
    assert(nr == _pgsz);
    nr = fread(p.data(), sizeof(int), _pgsz + 1, _file.get());
    assert(nr == _pgsz + 1);
    e.resize((unsigned long) pgh.m);
    p.resize((unsigned long) (pgh.m + 1));
    int fp = (int) ftell(_file.get());
    assert(fp % bpp == 0);
    return move(BNode{idx, pgh.parent, BPageBuf{e, p}});
}

void PagedFile::_write_page(const BNode &pg) {
    const BPageBuf &pgb = pg.buf();
    int idx = pg.idx();
    assert(idx >= 0);
    int rv = fseek(_file.get(), bpp * idx, SEEK_SET);
    assert(rv == 0);
    BPageHeader pgh;
    pgh.m = pgb.m();
    assert(pgh.m <= _pgsz);
    pgh.parent = pg.parent();
    int nr = fwrite(&pgh, sizeof(BPageHeader), 1, _file.get());
    assert(nr == 1);
    vector<BElement> e = pgb.ve();
    vector<int> p = pgb.vp();
    e.resize((unsigned long) _pgsz);
    p.resize((unsigned long) (_pgsz + 1));
    nr = fwrite(e.data(), sizeof(BElement), _pgsz, _file.get());
    assert(nr == _pgsz);
    nr = fwrite(p.data(), sizeof(int), _pgsz + 1, _file.get());
    assert(nr == _pgsz + 1);
    int fp = (int) ftell(_file.get());
    assert(fp % bpp == 0);
}

BNode &PagedFile::read_page(int idx) {
    BNode *cpg = _find_page(idx);
    if(cpg) {
        return *cpg;
    } else {
        _cache.emplace_back(new BNode(_read_page(idx)));
        BNode *ptr = _cache.back().get();
        assert(ptr);
        return *ptr;
    }
}

void PagedFile::write_back() {
    for(auto &pgp : _cache) {
        const BNode &pg = *pgp.get();
        _write_page(pg);
    }
    _cache.clear();
}
#endif
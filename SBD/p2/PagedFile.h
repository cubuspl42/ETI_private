#ifndef P2_FILE_H
#define P2_FILE_H

#include "BNode.h"

#include <algorithm>
#include <memory>
#include <string>

using namespace std;

enum PageStatus {
    PAGE_ALLOCATED,
    PAGE_PRESENT,
    PAGE_DELETED
};

struct PageMetaHeader {
    PageStatus status;
    int next;
};

class PagedFile {
    FilePtr _file;
    int _hdr_size;
    int _page_size;
    int _f = -1;
    vector<PageMetaHeader> _pagemap;

    int _page_offset(int i) {
        return _hdr_size + i * (_page_size);
    }

    void _fseek(int offset) {
        int rv = fseek(_file.get(), offset, SEEK_SET);
        assert(rv == 0);
    }

    void _fseek_page(int i) {
        int offset = _page_offset(_f);
        _fseek(offset);
    }

    template <typename T>
    void _fread(T *ptr) {
        int rv = fread(ptr, sizeof(T), 1, _file.get());
        assert(rv == 1);
    }

    template <typename T>
    void _fwrite(const T *ptr) {
        int rv = fwrite(ptr, sizeof(T), 1, _file.get());
        assert(rv == 1);
    }

    void _fwrite(const string &buf) {
        int rv = fwrite(buf.data(), sizeof(char), buf.size(), _file.get());
        assert(rv == (int) buf.size());
    }

public:
    PagedFile(string path, int hdr_size, int page_size)
        : _file{fopen(path.c_str(), "wb+"), file_close}, _hdr_size(hdr_size), _page_size(page_size) {

    }

    int alloc_page() {
        if(_f > -1) {
            int p = _f;
            PageMetaHeader &mh = _pagemap[p];
            assert(mh.status = PAGE_DELETED);
            _f = mh.next;
            mh.status = PAGE_ALLOCATED;
            mh.next = -1;
            return p;
        } else {
            _pagemap.push_back(PageMetaHeader{PAGE_ALLOCATED, -1});
            return (int) (_pagemap.size() - 1);
        }
    }

    void free_page(int i) {
        assert(i < (int) _pagemap.size());
        PageMetaHeader &mh = _pagemap[i];
        assert(mh.status == PAGE_PRESENT);
        assert(mh.next < 0);
        mh.status = PAGE_DELETED;
        mh.next = _f;
        _f = i;
    }

    void read_page(int i) {
        assert(i < (int) _pagemap.size());
        PageMetaHeader &mh = _pagemap[i];
        assert(mh.status == PAGE_PRESENT);
    }

    void write_page(int i, const string &buf) {
        assert(i < (int) _pagemap.size());
        assert((int) buf.size() == _page_size);
        PageMetaHeader &mh = _pagemap[i];
        assert(mh.status == PAGE_ALLOCATED || mh.status == PAGE_PRESENT);
        _fseek_page(i);
        _fwrite(buf);
        mh.status = PAGE_PRESENT;
    }

    template <typename Hdr>
    void read_header(Hdr *hdr) {
        assert(sizeof(Hdr) == _hdr_size);
        _fseek(0);
        _fread(hdr);
    }

    template <typename Hdr>
    void write_header(const Hdr *hdr) {
        assert(sizeof(Hdr) == _hdr_size);
        _fseek(0);
        _fwrite(hdr);
    }
};

#endif //P2_FILE_H

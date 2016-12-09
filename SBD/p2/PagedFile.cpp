#include "PagedFile.h"

#include <cassert>

static void file_close(FILE *file) {
    (void) fclose(file);
}

PagedFile::PagedFile(string path) : file{fopen(path.c_str(), "wb+"), file_close} {

}

BPage PagedFile::read_page(int idx) {
    assert(idx >= 0);
    return BPage();
}

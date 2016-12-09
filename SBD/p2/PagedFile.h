#ifndef P2_FILE_H
#define P2_FILE_H

#include "BPage.h"

#include <memory>
#include <string>

using namespace std;

using FilePtr = unique_ptr<FILE, void (*)(FILE *file)>;

class PagedFile {
    FilePtr file;
public:
    PagedFile(string path);
    BPage read_page(int idx);
};

#endif //P2_FILE_H

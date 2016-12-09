#include "IndexedFile.h"

IndexedFile::IndexedFile(string path) : pgf{path} {
}

void IndexedFile::insert(Record r) {

}

IndexedFile tmp_indexed_file() {
    return IndexedFile(tmpnam(nullptr));
}

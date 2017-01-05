//
// Created by kuba on 09.12.16.
//

#include "ContentFile.h"

#include <cassert>

ContentFile::ContentFile(string path) : file{fopen(path.c_str(), "rb+"), file_close} {
    cerr << "Loaded content file " << path << endl;
}

Record ContentFile::read_record(int i) {
    int rv;
    rv = fseek(file.get(), sizeof(Record) * i, SEEK_SET);
    assert(rv > -1);
    Record r;
    rv = fread(&r, sizeof(Record), 1, file.get());
    assert(rv == 1);
    return r;
}

int ContentFile::write_record(Record r) {
    int rv = fseek(file.get(), 0, SEEK_END);
    assert(rv == 0);
    int off = (int) ftell(file.get());
    assert(off % sizeof(Record) == 0);
    int a = off / sizeof(Record);
    int nr = fwrite(&r, sizeof(Record), 1, file.get());
    assert(nr == 1);
    return a;
}

void ContentFile::write_record(Record r, int i) {
    int rv;
    rv = fseek(file.get(), sizeof(Record) * i, SEEK_SET);
    assert(rv == 0);
    int nr = fwrite(&r, sizeof(Record), 1, file.get());
    assert(nr == 1);
}

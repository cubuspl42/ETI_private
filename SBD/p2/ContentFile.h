//
// Created by kuba on 09.12.16.
//

#ifndef P2_CONTENTFILE_H
#define P2_CONTENTFILE_H

#include "common.h"
#include "Record.h"

class ContentFile {
    FilePtr file;
public:
    ContentFile(string path);
    Record read_record(int i);
    int write_record(Record r);
};

#endif //P2_CONTENTFILE_H

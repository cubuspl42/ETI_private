//
// Created by kuba on 02.01.17.
//

#ifndef P2_FILESTORAGE_H
#define P2_FILESTORAGE_H


#include "BTreeStorage.h"

class FileStorage : public BTreeStorage {
    FilePtr file;

    bool check_header();
public:
    FileStorage(string path, string mode);

    virtual BTreeHeader read_header() override;

    virtual void write_header(BTreeHeader header) override;

    virtual void read_page(BNode &pg, int i) override;

    virtual void write_page(const BNode &pg, int i) override;
};


#endif //P2_FILESTORAGE_H

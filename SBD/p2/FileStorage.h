//
// Created by kuba on 02.01.17.
//

#ifndef P2_FILESTORAGE_H
#define P2_FILESTORAGE_H


#include "BTreeStorage.h"
#include "Metrics.h"

class FileStorage : public BTreeStorage {
    FilePtr file;

    bool check_header();
public:
    FileStorage(string path, string mode);

    virtual BTreeHeader read_header(Metrics *m) override;

    virtual void write_header(BTreeHeader header, Metrics *m) override;

    virtual void read_page(BNode &pg, int i, Metrics *m, string msg) override;

    virtual void write_page(const BNode &pg, int i, Metrics *m, string msg) override;
};


#endif //P2_FILESTORAGE_H

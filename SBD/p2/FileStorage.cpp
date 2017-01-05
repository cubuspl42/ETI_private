//
// Created by kuba on 02.01.17.
//

#include "FileStorage.h"

#include <string>

const size_t NODE_DATA_STORAGE_SIZE = 2 * D + 1;
static_assert(NODE_DATA_MEMORY_SIZE > NODE_DATA_STORAGE_SIZE,
              "NODE_DATA_MEMORY_SIZE <= NODE_DATA_STORAGE_SIZE");

const size_t SIZEOF_BTREE_HEADER = sizeof(BTreeHeader);
const size_t SIZEOF_NODE_DATA = NODE_DATA_STORAGE_SIZE * sizeof(Ep);
const size_t SIZEOF_NODE_HEADER = 2 * sizeof(int);
const size_t SIZEOF_NODE = SIZEOF_NODE_HEADER + SIZEOF_NODE_DATA;

FileStorage::FileStorage(string path) : file{fopen(path.c_str(), "rb+"), file_close} {
    if(!check_header()) {
        cerr << "FileStorage(" + path + "): Header not found; rewriting" << endl;
        BTreeHeader hdr{};
        write_header(hdr);
    } else {
        cerr << "FileStorage(" + path + "): Header found" << endl;
    }
}

bool FileStorage::check_header() {
    int rv;
    rv = fseek(file.get(), 0, SEEK_SET);
    assert(rv > -1);
    BTreeHeader hdr;
    rv = fread(&hdr, sizeof(hdr), 1, file.get());
    return (rv == 1);
}


BTreeHeader FileStorage::read_header() {
    int rv;
    rv = fseek(file.get(), 0, SEEK_SET);
    assert(rv > -1);
    BTreeHeader hdr;
    rv = fread(&hdr, sizeof(hdr), 1, file.get());
    assert(rv == 1);
    return hdr;
}

void FileStorage::write_header(BTreeHeader header) {
    int rv;
    rv = fseek(file.get(), 0, SEEK_SET);
    assert(rv > -1);
    rv = fwrite(&header, sizeof(header), 1, file.get());
    assert(rv == 1);
}

void FileStorage::read_page(BNode &nd, int i) {
    int rv;
    rv = fseek(file.get(), SIZEOF_BTREE_HEADER + i * SIZEOF_NODE, SEEK_SET);
    assert(rv > -1);
    int t[2];
    rv = fread(t, sizeof(int), 2, file.get());
    assert(rv == 2);
    nd.idx = t[0];
    nd.m = t[1];
    assert(nd.data.size() == NODE_DATA_MEMORY_SIZE);
    rv = fread(nd.data.data(), sizeof(Ep), NODE_DATA_STORAGE_SIZE, file.get());
    assert(rv == NODE_DATA_STORAGE_SIZE);
}

void FileStorage::write_page(const BNode &nd, int i) {
    int rv;
    assert(i >= 0);
    rv = fseek(file.get(), SIZEOF_BTREE_HEADER + i * SIZEOF_NODE, SEEK_SET);
    assert(rv > -1);
    int t[2] = {nd.idx, nd.m};
    rv = fwrite(t, sizeof(int), 2, file.get());
    assert(rv == 2);
    assert(nd.data.size() == NODE_DATA_MEMORY_SIZE);
    rv = fwrite(nd.data.data(), sizeof(Ep), NODE_DATA_STORAGE_SIZE, file.get());
    assert(rv == NODE_DATA_STORAGE_SIZE);
}
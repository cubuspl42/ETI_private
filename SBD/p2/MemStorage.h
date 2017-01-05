//
// Created by kuba on 18.12.16.
//

#ifndef P2_MEMSTORAGE_H
#define P2_MEMSTORAGE_H


#include "BTreeStorage.h"

class MemStorage : public BTreeStorage {
    BTreeHeader _hdr;
    vector<BNode> _nodes;
public:
    MemStorage() = default;

    MemStorage(BTreeHeader hdr, vector<BNode> nodes) : _hdr(hdr), _nodes(move(nodes)) {
        for(int i = 0; i < (int) _nodes.size(); ++i) {
            if(_nodes[i].m > 0) _nodes[i].idx = i;
        }
    }

    virtual BTreeHeader read_header() override;

    virtual void write_header(BTreeHeader header) override;

    virtual void read_page(BNode &pg, int i) override;

    virtual void write_page(const BNode &pg, int i) override;

    bool operator==(const MemStorage &o);

    bool operator!=(const MemStorage &o);
};


#endif //P2_MEMSTORAGE_H

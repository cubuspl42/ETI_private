//
// Created by kuba on 18.12.16.
//

#ifndef P2_MEMSTORAGE_H
#define P2_MEMSTORAGE_H


#include "BTreeStorage.h"
#include "Metrics.h"

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

    virtual BTreeHeader read_header(Metrics *m) override;

    virtual void write_header(BTreeHeader header, Metrics *m) override;

    virtual void read_page(BNode &pg, int i, Metrics *m) override;

    virtual void write_page(const BNode &pg, int i, Metrics *m) override;

    bool operator==(const MemStorage &o);

    bool operator!=(const MemStorage &o);
};


#endif //P2_MEMSTORAGE_H

#include "BPage.h"

BPage::BPage(int idx, int parent, BPageBuf buf)
        : _idx(idx), _parent(parent), _buf(move(buf)) {
}

void BPage::reset(int parent, BPageBuf buf) {
    _parent = parent;
    _buf = move(buf);
}

void BPage::reset(BPageBuf buf) {
    reset(_parent, move(buf));
}

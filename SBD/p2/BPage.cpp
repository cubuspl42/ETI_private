#include "BPage.h"

BPage::BPage(int idx, int parent, vector<BEntry> data)
    : _idx(idx), _parent(parent), _data(move(data)) {
}

void BPage::reset(int parent, vector<BEntry> data) {
    _parent = parent;
    _data = move(data);
}

void BPage::insert(BEntry e) {
    assert(find(e.x) == NOT_FOUND);
    _data.push_back(e);
    sort(_data.begin(), _data.end());
}

tuple<vector<BEntry>, BEntry, vector<BEntry>> BPage::split() {
    assert(m() > 3);
    int med = (m() - 1) / 2;
    vector<BEntry> ldata{_data.begin(), _data.begin() + med};
    vector<BEntry> rdata{_data.begin() + med + 1, _data.end()};
    return make_tuple(move(ldata), _data[med], move(rdata));
}

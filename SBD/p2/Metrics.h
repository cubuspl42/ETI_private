//
// Created by kuba on 05.01.17.
//

#ifndef P2_METRICS_H
#define P2_METRICS_H

struct Metrics {
    bool enabled = true;
    int page_reads = 0;
    int page_writes = 0;
    int header_reads = 0;
    int header_writes = 0;

    void inc_page_reads() {
        if(enabled) ++page_reads;
    }

    void inc_page_writes() {
        if(enabled) ++page_writes;
    }

    void inc_header_reads() {
        if(enabled) ++header_reads;
    }

    void inc_header_writes() {
        if(enabled) ++header_writes;
    }

    void reset() {
        page_reads = 0;
        page_writes = 0;
        header_reads = 0;
        header_writes = 0;
    }

    void dump();
};

#endif //P2_METRICS_H

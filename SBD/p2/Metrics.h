//
// Created by kuba on 05.01.17.
//

#ifndef P2_METRICS_H
#define P2_METRICS_H


struct Metrics {
    int page_reads = 0;
    int page_writes = 0;
    int header_reads = 0;
    int header_writes = 0;
};

extern Metrics metrics;

inline void reset_metrics() {
    metrics = Metrics{};
}

#endif //P2_METRICS_H

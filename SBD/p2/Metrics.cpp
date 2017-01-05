//
// Created by kuba on 05.01.17.
//

#include "Metrics.h"

#include <iostream>

using namespace std;

void Metrics::dump() {
    cout << "Metrics: " << endl;
    cout << "header reads: " << header_reads << " / ";
    cout << "header writes: " << header_writes << " / ";
    cout << "page reads: " << page_reads << " / ";
    cout << "page writes: " << page_writes << endl;
}

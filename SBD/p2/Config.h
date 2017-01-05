//
// Created by kuba on 05.01.17.
//

#ifndef P2_CONFIG_H
#define P2_CONFIG_H

#include <string>

using namespace std;

struct Config {
    string indexed_file_path;
    string cmd_file_path;
    bool print_intermediate = false;
    bool verbose = false;
    bool metrics = false;
};

extern Config cfg;

#endif //P2_CONFIG_H

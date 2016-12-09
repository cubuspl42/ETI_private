#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <climits>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <fstream>
#include <math.h>
#include "PagedFile.h"
#include "Record.h"
#include "IndexedFile.h"

#define DBG 0
#if DBG
#define dprintf printf
#else
#define dprintf nullprintf
#endif

using namespace std;

struct Config {
    string indexed_file_path;
    string cmd_file_path;
};

Config cfg;

void die(string message) {
    cerr << message << endl;
    std::exit(1);
}

vector<Record> read_records(istream &is, int n) {
    vector<Record> v;
    for(int i = 0; i < n; ++i) {
        Record r;
        is >> r;
        v.push_back(r);
    }
    return v;
}

void exec_commands(IndexedFile &idf, istream &is_cmd) {
    while(is_cmd.good()) {
        string cmd;
        is_cmd >> cmd;

        if(cmd == "insert") {
            Record r;
            is_cmd >> r;
            assert(!is_cmd.fail());
            cout << "INSERT " << r << endl;
            idf.insert(r);
        } else if(cmd == "check") {
            int n;
            is_cmd >> n;
            vector<Record> v_e = read_records(is_cmd, n);
            vector<Record> v_a = idf.to_vector();
            cout << "check: " << (v_e == v_a ? "ok" : "fail") << endl;
        } else if(cmd == "print") {
            cout << "print:" << endl;
            idf.for_each([&](Record r) {
                cout << r << endl;
            });
        } else {
            die("Invalid command: " + cmd);
        }
    }
    assert(!is_cmd.fail());
}

void parse_argv(int argc, const char **argv) {
    for(int i = 0; i < argc; ++i) {
//        if(string{argv[i]} == "-p") cfg.print_intermediate = true;
        if(i < argc - 1) {
//            if(string{argv[i]} == "-r") cfg.n_rand = atoi(argv[i+1]);
            if(string{argv[i]} == "-i") cfg.indexed_file_path = argv[i+1];
            if(string{argv[i]} == "-t") cfg.cmd_file_path = argv[i+1];
//            if(string{argv[i]} == "-b") cfg.b = atoi(argv[i+1]);
//            if(string{argv[i]} == "-n") cfg.n = atoi(argv[i+1]);
        }
    }
}

/**
 * Flags:
 * -r <n> -- generate <n> random records
 * -t <filename> -- read test commands from file <filename>
 * -b <value> -- specify records/page number
 * -n <value> -- specify number of buffers
 * -p -- print intermediate states
 */
int main(int argc, const char *argv[]) {
    parse_argv(argc, argv);

    IndexedFile idf = cfg.indexed_file_path.size() ?
                      IndexedFile{cfg.indexed_file_path} :
                      tmp_indexed_file();

    if(cfg.cmd_file_path.size()) {
        ifstream ifs_cmd{cfg.cmd_file_path};
        assert(ifs_cmd.good());
        exec_commands(idf, ifs_cmd);
    } else {
        exec_commands(idf, cin);
    }

    return 0;
}

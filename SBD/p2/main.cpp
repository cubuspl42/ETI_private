#include "Record.h"
#include "IndexedFile.h"
#include "Metrics.h"
#include "Config.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <map>

#define DBG 0
#if DBG
#define dprintf printf
#else
#define dprintf nullprintf
#endif

using namespace std;

void die(string message) {
    cerr << message << endl;
    exit(1);
}

vector<pair<int, Record>> read_records(istream &is, int n) {
    vector<pair<int, Record>> v;
    for (int i = 0; i < n; ++i) {
        int k;
        Record r;
        is >> k >> r;
        v.push_back({k, r});
    }
    return v;
}

static vector<pair<int, Record>> map2vec(const map<int, Record> &s) {
    vector<pair<int, Record>> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    return v;
}

static vector<pair<int, Record>> idf2vec(IndexedFile &idf) {
    vector<pair<int, Record>> v;
    idf.for_each([&](int k, Record r) {
        v.push_back({k, r});
    }, nullptr);
    return v;
}

static void dump_vec(const vector<pair<int, Record>> &v) {
    cout << "[ ";
    for (auto p : v) {
        cout << p.first << ":" << p.second << " ";
    }
    cout << "]";
}

static void check(const map<int, Record> &s, IndexedFile &idf) {
    auto sv = map2vec(s);
    auto btv = idf2vec(idf);
    if (sv != btv) {
        cout << "sv: ";
        dump_vec(sv);
        cout << endl << endl;

        cout << "idf:" << endl;
        idf.dump();
        cout << endl;

        cout << "btv: ";
        dump_vec(btv);
        cout << endl << endl;
    }
    assert(sv == btv);
}

void exec_commands(IndexedFile &idf, istream &is_cmd) {
    map<int, Record> mp;
    idf.for_each([&](int k, Record r) {
        mp.insert({k, r});
    }, nullptr);
    while (is_cmd.good()) {
        string cmd;
        is_cmd >> cmd;

        Metrics metrics;
        Metrics *m = cfg.metrics ? &metrics : nullptr;

        if (cmd == "insert") {
            int k;
            Record r;
            is_cmd >> k >> r;
            assert(!is_cmd.fail());
            cout << "INSERT " << k << " -> " << r << endl;

            idf.insert(k, r, m);

            mp.insert({k, r});
            check(mp, idf);
        } else if (cmd == "remove") {
            int k;
            is_cmd >> k;
            assert(!is_cmd.fail());
            cout << "REMOVE " << k << endl;

            Record r = idf.remove(k, m);

            cout << "Record removed: " << r << endl;
            mp.erase(k);
            check(mp, idf);
        } else if (cmd == "find") {
            int k;
            is_cmd >> k;
            assert(!is_cmd.fail());
            cout << "FIND " << k << endl;

            auto p = idf.find(k, m);

            if (p.first) {
                cout << "Record found: " << p.second << endl;
            } else {
                cout << "Record not found" << endl;
            }
            check(mp, idf);
        } else if (cmd == "update") {
            int k;
            Record r;
            is_cmd >> k >> r;
            assert(!is_cmd.fail());
            cout << "UPDATE " << k << " -> " << r << endl;

            Record olr = idf.update(k, r, m);

            mp.erase(k);
            mp.insert({k, r});
            cout << "Record updated. Old record: " << olr << endl;
            check(mp, idf);
        } else if (cmd == "check") {
            int n;
            is_cmd >> n;
            auto v_e = read_records(is_cmd, n);
            auto v_a = idf.to_vector();
            cout << "check: " << (v_e == v_a ? "ok" : "fail") << endl;
        } else if (cmd == "print") {
            cout << "print:" << endl;
            dump_vec(idf2vec(idf));
            cout << endl;
            cout << "dump:" << endl;
            idf.dump();
        } else {
            die("Invalid command: " + cmd);
        }

        if(m) {
            m->dump();
        }

        if(cfg.print_intermediate) {
            idf.dump();
        }

        cout << endl;
    }
    assert(!is_cmd.fail());
}

void parse_argv(int argc, const char **argv) {
    for (int i = 0; i < argc; ++i) {
        if(string{argv[i]} == "-v") cfg.verbose = true;
        if(string{argv[i]} == "-p") cfg.print_intermediate = true;
        if(string{argv[i]} == "-m") cfg.metrics = true;
        if(string{argv[i]} == "-e") cfg.experiment = true;
        if (i < argc - 1) {
            if (string{argv[i]} == "-i") cfg.indexed_file_path = argv[i + 1];
            if (string{argv[i]} == "-t") cfg.cmd_file_path = argv[i + 1];
        }
    }
}

IndexedFile load_indexed_file(bool tmp) {
    if(tmp) {
        return IndexedFile(tmpnam(nullptr), "wb+");
    } else {
        return IndexedFile{cfg.indexed_file_path, "rb+"};
    }
}

static void experiment(IndexedFile &idf) {
    cout << "D = " << D << endl;

    int N = 16000;
    int nr = 1000;
    for(int i = 1; i <= N; ++i) {
        Record r{0, 0, 0, 0, 0, i};
        idf.insert(i, r, nullptr);
    }

    Metrics m;
    for(int i = 0; i < nr; ++i) {
        idf.find(rand() % N, &m);
    }

    m.dump();
    cout << m.page_reads / (double) nr << "/" << m.page_writes / (double) nr << endl;
}

/**
 * Flags:
 * -t <filename> -- read test commands from file <filename>
 * -i <filename> -- reads content file <filename> and its index <filename>_index
 * -m -- print metrics
 * -p -- print intermediate states
 * -v -- print verbose info (works with -m)
 */
int main(int argc, const char *argv[]) {
    parse_argv(argc, argv);

    IndexedFile idf = load_indexed_file(cfg.indexed_file_path.empty());

    if(cfg.experiment) {
        experiment(idf);
    } else if (cfg.cmd_file_path.size()) {
        ifstream ifs_cmd{cfg.cmd_file_path};
        assert(ifs_cmd.good());
        exec_commands(idf, ifs_cmd);
    } else {
        exec_commands(idf, cin);
    }

    return 0;
}

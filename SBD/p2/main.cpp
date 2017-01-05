#include "Record.h"
#include "IndexedFile.h"
#include "Metrics.h"

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
#include <map>

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
    bool print_intermediate = false;
    bool verbose = false;
};

Config cfg;

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

        Metrics m;

        if (cmd == "insert") {
            int k;
            Record r;
            is_cmd >> k >> r;
            assert(!is_cmd.fail());
            cout << endl << "INSERT " << k << " -> " << r << endl;

            idf.insert(k, r, &m);

            mp.insert({k, r});
            check(mp, idf);
        } else if (cmd == "remove") {
            int k;
            is_cmd >> k;
            assert(!is_cmd.fail());
            cout << endl << "REMOVE " << k << endl;

            Record r = idf.remove(k, &m);

            cout << "Record removed: " << r << endl;
            mp.erase(k);
            check(mp, idf);
        } else if (cmd == "find") {
            int k;
            is_cmd >> k;
            assert(!is_cmd.fail());
            cout << endl << "FIND " << k << endl;

            auto p = idf.find(k, &m);

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
            cout << endl << "UPDATE " << k << " -> " << r << endl;

            Record olr = idf.update(k, r, &m);

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

        cout << "Metrics: " << endl;
        cout << "header reads: " << m.header_reads << " / ";
        cout << "header writes: " << m.header_writes << " / ";
        cout << "page reads: " << m.page_reads << " / ";
        cout << "page writes: " << m.page_writes << endl;

        if(cfg.print_intermediate) {
            idf.dump();
        }
    }
    assert(!is_cmd.fail());

    // TODO: przeglądania całej zawartości pliku i indeksu zgodnie z kolejnością wartości klucza
}

void parse_argv(int argc, const char **argv) {
    for (int i = 0; i < argc; ++i) {
        if(string{argv[i]} == "-v") cfg.verbose = true;
        if(string{argv[i]} == "-p") cfg.print_intermediate = true;
        if (i < argc - 1) {
//            if(string{argv[i]} == "-r") cfg.n_rand = atoi(argv[i+1]);
            if (string{argv[i]} == "-i") cfg.indexed_file_path = argv[i + 1];
            if (string{argv[i]} == "-t") cfg.cmd_file_path = argv[i + 1];
//            if(string{argv[i]} == "-b") cfg.b = atoi(argv[i+1]);
//            if(string{argv[i]} == "-n") cfg.n = atoi(argv[i+1]);
        }
    }

    // TODO: Program powinien dawać możliwość wyświetlania zawartości pliku i indeksu po
    // każdej operacji zmieniającej zawartość pliku (tj. po wstawieniu, aktualizacji lub usunięciu rekordu)
}

IndexedFile load_indexed_file(bool tmp) {
    if(tmp) {
        return IndexedFile(tmpnam(nullptr), "wb+");
    } else {
        return IndexedFile{cfg.indexed_file_path, "rb+"};
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

    IndexedFile idf = load_indexed_file(cfg.indexed_file_path.empty());

    if (cfg.cmd_file_path.size()) {
        ifstream ifs_cmd{cfg.cmd_file_path};
        assert(ifs_cmd.good());
        exec_commands(idf, ifs_cmd);
    } else {
        exec_commands(idf, cin);
    }

    return 0;
}

#ifndef P2_COMMON_H_H
#define P2_COMMON_H_H

#include <cstdio>
#include <memory>
#include <assert.h>

constexpr int NIL = -1;
constexpr int NOT_FOUND = -1;
const int D = 2;

using namespace std;

enum InsertStatus {
    OK,
    ALREADY_EXISTS
};

using FilePtr = unique_ptr<FILE, void (*)(FILE *file)>;

inline FILE * file_open(string path, string mode) {
    FILE *f = fopen(path.c_str(), mode.c_str());
    assert(f);
    return f;
}

inline void file_close(FILE *file) {
    (void) fclose(file);
}


#endif //P2_COMMON_H_H

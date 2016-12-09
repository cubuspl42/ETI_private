#ifndef P2_COMMON_H_H
#define P2_COMMON_H_H

#include <cstdio>
#include <memory>

constexpr int NIL = -1;
constexpr int NOT_FOUND = -1;

using namespace std;

enum InsertStatus {
    OK,
    ALREADY_EXISTS
};

using FilePtr = unique_ptr<FILE, void (*)(FILE *file)>;

inline void file_close(FILE *file) {
    (void) fclose(file);
}


#endif //P2_COMMON_H_H

#include "Record.h"

int64_t Record::pkey() {
    return g(*this);
}

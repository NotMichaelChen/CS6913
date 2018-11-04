#include "utility.h"

#include <math.h>

inline int util_getDigitCount(size_t num) {
    return (num == 0) ? 2 : (sizeof(char)*(int)log10(num))+2;
}

// Comparison functions

int util_gcmpHeapEntry(const void* a, const void* b) {
    const HeapEntry *c = b, *d = a;

    return (c->score > d->score) - (c->score < d->score);
}
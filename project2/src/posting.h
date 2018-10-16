#pragma once

#include <stdint.h>

#include "michaellib/string.h"

typedef struct IntermediatePosting {
    String* term;
    uint32_t freq;
} IntermediatePosting;

typedef struct MemPosting {
    String* term;
    size_t docID;
    size_t freq;
} MemPosting;

typedef struct DiskPosting {
    size_t docID;
    size_t freq;
} DiskPosting;

int memposting_cmp(const MemPosting* a, const MemPosting* b);
// Return the size of the posting if it were to be written to a file
size_t memposting_getSize(MemPosting* posting);
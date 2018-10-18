#pragma once

#include <stdint.h>

#include "michaellib/string.h"

typedef struct IntermediatePosting {
    String* term;
    size_t freq;
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

int memposting_cmp(const void* a, const void* b);
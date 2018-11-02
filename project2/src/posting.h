#pragma once

#include <stdint.h>
#include "global.h"

#include "michaellib/string.h"

// Contains posting structs that hold various forms of postings

typedef struct IntermediatePosting {
    String* term;
    freq_t freq;
} IntermediatePosting;

typedef struct MemPosting {
    String* term;
    docID_t docID;
    freq_t freq;
} MemPosting;

typedef struct DiskPosting {
    docID_t docID;
    freq_t freq;
} DiskPosting;

int memposting_cmp(const void* a, const void* b);
#pragma once

#include <stdint.h>

#include "michaellib/string.h"

typedef struct IntermediatePosting {
    String* term;
    uint32_t freq;
} IntermediatePosting;

typedef struct MemPosting {
    String* term;
    uint32_t docID;
    uint32_t freq;
} MemPosting;

typedef struct DiskPosting {
    uint32_t docID;
    uint32_t freq;
} DiskPosting;
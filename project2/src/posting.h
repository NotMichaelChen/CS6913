#pragma once

#include <stdint.h>

#include "michaellib/string.h"

typedef struct IntermediatePosting {
    String* term;
    uint32_t docID;
} IntermediatePosting;

typedef struct Posting {
    uint32_t docID;
    uint32_t freq;
} Posting;
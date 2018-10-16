#pragma once

#include "posting.h"
#include "reader/reader.h"

typedef struct IntermediatePostingList {
    IntermediatePosting* head;
    size_t len;
} IntermediatePostingList;

IntermediatePostingList docparser_getPostings(Document doc);

void docparser_freeIntermPostingList(IntermediatePostingList postinglist);
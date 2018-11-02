#pragma once

#include "posting.h"
#include "reader/reader.h"

// Provides functions to get an intermediate posting list from a document. Here,
// intermediate postings contain only the term and frequency, since we assign
// the docID in a higher level function.

// Represents a posting list allocated on the heap
typedef struct {
    MemPosting* head;
    size_t len;
} MemPostingList;

// Gets the intermediate posting list from a document
MemPostingList docparser_getPostings(Document doc, docID_t docID);

// Frees the intermediate posting list, since the actual posting list array is
// allocated on the heap, and each posting contains a malloc'd string which
// must be freed.
void docparser_freePostingList(MemPostingList postinglist);
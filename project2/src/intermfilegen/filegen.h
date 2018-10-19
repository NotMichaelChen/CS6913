#pragma once

#include "reader/reader.h"

#include <stdint.h>

// Represents an object that writes postings to disk. Manages postings,
// buffering them, and writing them out to the next file, along with managing
// the next valid docID

typedef struct PostingGenerator PostingGenerator;

PostingGenerator* postinggen_new(char* directory, uint32_t buffer);

// Add a document to the intermediate collection. Should be linked directly with
// the dirreader object's getDocument function
void postinggen_addDoc(PostingGenerator* postinggen, Document doc);
// Flushes the posting generator's buffer to disk.
void postinggen_flush(PostingGenerator* postinggen);

void postinggen_free(PostingGenerator* postinggen);
#pragma once

#include "reader/reader.h"

#include <stdint.h>

typedef struct PostingGenerator PostingGenerator;

PostingGenerator* postinggen_new(char* directory, uint32_t buffer);

void postinggen_addDoc(PostingGenerator* postinggen, Document doc);
void postinggen_flush(PostingGenerator* postinggen);

void postinggen_free(PostingGenerator* postinggen);
#pragma once

#include <stdint.h>

#include "reader/reader.h"
#include "posting.h"

typedef struct DocTokenizer DocTokenizer;

DocTokenizer* doctokenizer_new(Document doc, uint32_t docID);

IntermediatePosting doctokenizer_getPosting(DocTokenizer* parser);
int doctokenizer_getErr(DocTokenizer* parser);

void doctokenizer_free(DocTokenizer* parser);
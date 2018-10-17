#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "posting.h"

typedef struct PostingVector PostingVector;

PostingVector* postingvector_new(size_t bytecap);

size_t postingvector_getBytesRemaining(PostingVector* vec);
void postingvector_insert(PostingVector* vec, size_t docID, size_t freq, char* term);
void postingvector_sortflush(PostingVector* vec, FILE* fp);

void postingvector_free(PostingVector* vec);
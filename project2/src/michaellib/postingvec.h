#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "posting.h"

typedef struct PostingVector PostingVector;

PostingVector* postingvector_new(size_t bytecap);

size_t postingvector_getBytesRemaining(PostingVector* vec);
//Makes a copy of the posting, so original posting must be deleted
void postingvector_insert(PostingVector* vec, MemPosting* posting);
void postingvector_sortflush(PostingVector* vec, FILE* fp);

void postingvector_free(PostingVector* vec);
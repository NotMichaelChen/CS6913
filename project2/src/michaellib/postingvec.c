#include "postingvec.h"

#include <string.h>
#include <stdio.h>

#include "michaellib/utility.h"

struct PostingVector {
    MemPosting* buf;
    size_t size;
    size_t capacity;

    size_t byteremaining;
    size_t bytecap;
};

PostingVector* postingvector_new(size_t bytecap) {
    PostingVector* vec = malloc(sizeof(PostingVector));

    vec->buf = NULL;
    vec->size = vec->capacity = 0;
    vec->bytecap = vec->byteremaining = bytecap;

    return vec;
}

size_t postingvector_getBytesRemaining(PostingVector* vec) {
    return vec->byteremaining;
}

void postingvector_insert(PostingVector* vec, size_t docID, size_t freq, char* term) {
    // Add 2 spaces, add 1 newline, subtract 2 from both getDigitcount
    size_t postingsize = util_getMempostingSize(docID, freq, strlen(term));
    if(postingsize > vec->byteremaining)
        return;
    
    if(vec->size == vec->capacity) {
        //realloc
        if(vec->capacity == 0) {
            vec->capacity = 1;
            vec->buf = malloc(sizeof(MemPosting) * vec->capacity);
        }
        else {
            vec->capacity *= 2;
            vec->buf = realloc(vec->buf, sizeof(MemPosting) * vec->capacity);
        }
    }

    vec->buf[vec->size].docID = docID;
    vec->buf[vec->size].freq = freq;
    vec->buf[vec->size].term = string_newstr(term);

    vec->size += 1;
    vec->byteremaining -= postingsize;
}

void postingvector_sortflush(PostingVector* vec, FILE* fp) {
    qsort(vec->buf, vec->size, sizeof(MemPosting), memposting_cmp);

    for(size_t i = 0; i < vec->size; i++) {
        fprintf(fp, "%s %lu %lu\n",
            string_getString(vec->buf[i].term),
            vec->buf[i].docID,
            vec->buf[i].freq
        );
    }

    for(size_t i = 0; i < vec->size; i++) {
        string_free(vec->buf[i].term);
    }
    vec->size = 0;
    vec->byteremaining = vec->bytecap;
}

void postingvector_free(PostingVector* vec) {
    for(size_t i = 0; i < vec->size; i++) {
        string_free(vec->buf[i].term);
    }

    free(vec->buf);
    free(vec);
}
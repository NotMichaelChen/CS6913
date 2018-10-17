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
        String* line = string_newstr(string_getString(vec->buf[i].term));
        string_appendString(line, " ", 1);

        uint32_t docIDlen = util_getDigitCount(vec->buf[i].docID);
        char* docIDstr = malloc(docIDlen);
        snprintf(docIDstr, docIDlen, "%lu", vec->buf[i].docID);

        size_t freq = vec->buf[i].freq;
        uint32_t freqlen = util_getDigitCount(freq);
        char* freqstr = malloc(freqlen);
        snprintf(freqstr, freqlen, "%lu", freq);

        string_appendString(line, docIDstr, docIDlen-1);
        string_appendString(line, " ", 1);
        string_appendString(line, freqstr, freqlen-1);
        string_appendString(line, "\n", 1);

        size_t linelen = string_getLen(line);

        fwrite(string_getString(line), 1, linelen, fp);
        // fputs(string_getString(line), fp);

        free(docIDstr);
        free(freqstr);
        string_free(line);
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
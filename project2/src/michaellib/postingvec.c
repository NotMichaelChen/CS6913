#include "postingvec.h"

#include "michaellib/utility.h"

struct PostingVector {
    MemPosting* buf;
    size_t size;
    size_t capacity;

    size_t bytesize;
    size_t bytecap;
};

PostingVector* postingvector_new(size_t bytecap) {
    PostingVector* vec = malloc(sizeof(PostingVector));

    vec->buf = NULL;
    vec->size = vec->capacity = vec->bytesize = 0;
    vec->bytecap = bytecap;

    return vec;
}

size_t postingvector_getBytesRemaining(PostingVector* vec) {
    return vec->bytecap - vec->bytesize;
}

void postingvector_insert(PostingVector* vec, MemPosting* posting) {
    if(memposting_getSize(posting) > vec->bytecap - vec->bytesize)
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

    vec->buf[vec->size].docID = posting->docID;
    vec->buf[vec->size].freq = posting->freq;
    vec->buf[vec->size].term = string_newstr(string_getString(posting->term));

    vec->size += 1;
    vec->bytesize += memposting_getSize(posting);
}

void postingvector_sortflush(PostingVector* vec, FILE* fp) {
    qsort(vec->buf, vec->size, sizeof(MemPosting), memposting_cmp);

    for(size_t i = 0; i < vec->size; i++) {
        String* line = string_newstr(string_getString(vec->buf[i].term));
        string_appendString(line, " ", 1);

        uint32_t docIDlen = util_getDigitCount(vec->buf[i].docID);
        char* docIDstr = malloc(docIDlen);
        snprintf(docIDstr, docIDlen, "%d", vec->buf[i].docID);

        uint32_t freq = vec->buf[i].freq;
        uint32_t freqlen = util_getDigitCount(freq);
        char* freqstr = malloc(freqlen);
        snprintf(freqstr, freqlen, "%d", freq);

        string_appendString(line, docIDstr, docIDlen-1);
        string_appendString(line, " ", 1);
        string_appendString(line, freqstr, freqlen-1);
        string_appendString(line, "\n", 1);

        size_t linelen = string_getLen(line);

        fputs(string_getString(line), fp);

        free(docIDstr);
        free(freqstr);
        string_free(line);
    }

    for(size_t i = 0; i < vec->size; i++) {
        string_free(vec->buf[i].term);
    }
    vec->size = 0;
    vec->bytesize = 0;
}

void postingvector_free(PostingVector* vec) {
    for(size_t i = 0; i < vec->size; i++) {
        string_free(vec->buf[i].term);
    }

    free(vec->buf);
    free(vec);
}
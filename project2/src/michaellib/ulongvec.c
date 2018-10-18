#include "ulongvec.h"

struct ULongVector {
    uint64_t* buf;
    size_t capacity;
    size_t size;
};

ULongVector* ulongvector_new() {
    ULongVector* vec = malloc(sizeof(ULongVector));

    vec->buf = NULL;
    vec->size = 0;
    vec->capacity = 0;

    return vec;
}

void ulongvector_append(ULongVector* vec, uint64_t num) {
    if (vec->size == 0) {
        vec->capacity = 1;
        vec->buf = malloc(sizeof(uint64_t) * vec->capacity);
    }

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->buf = realloc(vec->buf, sizeof(uint64_t) * vec->capacity);
    }

    vec->buf[vec->size] = num;
    vec->size++;
}

uint64_t ulongvector_get(ULongVector* vec, size_t pos) {
    return vec->buf[pos];
}

size_t ulongvector_size(ULongVector* vec) {
    return vec->size;
}

uint64_t* ulongvector_getBuffer(ULongVector* vec) {
    return vec->buf;
}

void ulongvector_clear(ULongVector* vec) {
    vec->size = 0;
}

void ulongvector_free(ULongVector* vec) {
    free(vec->buf);
    free(vec);
}
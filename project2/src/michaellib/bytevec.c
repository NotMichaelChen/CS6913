#include "bytevec.h"

#include <string.h>

typedef struct ByteVec {
    uint8_t* buffer;
    size_t capacity;
    size_t size;
} ByteVec;

ByteVec* bytevec_new() {
    ByteVec* vec = malloc(sizeof(ByteVec*));

    vec->buffer = NULL;
    vec->size = 0;
    vec->capacity = 0;

    return vec;
}

void bytevec_append(ByteVec* vec, uint8_t byte) {
    if (vec->capacity == 0) {
        vec->capacity = 1;
        vec->buffer = malloc(sizeof(uint8_t) * vec->capacity);
    }

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        uint8_t* tmp = realloc(vec->buffer, sizeof(uint8_t) * vec->capacity);
        if(tmp == NULL) {
            fprintf(stderr, "Error: realloc call in bytevec_append failed\n");
            exit(1);
        }
        else {
            vec->buffer = tmp;
        }
    }

    vec->buffer[vec->size] = byte;
    vec->size++;
}

void bytevec_appendstr(ByteVec* vec, String* str) {
    if (vec->capacity == 0) {
        vec->capacity = 1;
        vec->buffer = malloc(sizeof(uint8_t) * vec->capacity);
    }

    // Realloc until large enough
    while (vec->size + string_getLen(str) >= vec->capacity) {
        vec->capacity *= 2;
        uint8_t* tmp = realloc(vec->buffer, sizeof(uint8_t) * vec->capacity);
        if(tmp == NULL) {
            fprintf(stderr, "Error: realloc call in bytevec_append failed\n");
            exit(1);
        }
        else {
            vec->buffer = tmp;
        }
    }

    memcpy(vec->buffer + vec->size, string_getString(str), string_getLen(str));
    vec->size += string_getLen(str);
}

uint8_t bytevec_get(ByteVec* vec, size_t pos) {
    return vec->buffer[pos];
}

size_t bytevec_len(ByteVec* vec) {
    return vec->size;
}

void bytevec_clear(ByteVec* vec) {
    vec->size = 0;
}

void bytevec_dump(ByteVec* vec, FILE* fp) {
    fwrite(vec->buffer, 1, vec->size, fp);
}

void bytevec_free(ByteVec* vec) {
    free(vec->buffer);
    free(vec);
}
#include "stringvec.h"

struct StringVec {
    char** buf;
    size_t capacity;
    size_t length;
};

StringVec* stringvec_new() {
    StringVec* stringvec = malloc(sizeof(StringVec));

    stringvec->buf = NULL;
    stringvec->length = 0;
    stringvec->capacity = 0;

    return stringvec;
}

void stringvec_append(StringVec* stringvec, char* str) {
    if (stringvec->capacity == 0) {
        stringvec->capacity = 1;
        stringvec->buf = malloc(sizeof(char*) * stringvec->capacity);
    }

    if (stringvec->length == stringvec->capacity) {
        stringvec->capacity *= 2;
        char** tmp = realloc(stringvec->buf, sizeof(char*) * stringvec->capacity);
        if(tmp == NULL) {
            printf("Error: realloc call in stringvec_append failed\n");
            exit(1);
        }
        else {
            stringvec->buf = tmp;
        }
    }

    stringvec->buf[stringvec->length] = str;
    stringvec->length++;
}

size_t stringvec_len(StringVec* stringvec) {
    return stringvec->length;
}

char* stringvec_getstr(StringVec* stringvec, size_t index) {
    return stringvec->buf[index];
}

char** stringvec_getbuffer(StringVec* stringvec) {
    return stringvec->buf;
}

void stringvec_free(StringVec* stringvec) {
    free(stringvec->buf);
    free(stringvec);
}
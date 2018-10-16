#include "filegen.h"

#include <stdlib.h>

#include "michaellib/string.h"

struct PostingGenerator {
    char* buffer;
    uint32_t bufsize;

    uint32_t nextdocID;

    String* dir;
    int err;
    int nextfilenum;
};


PostingGenerator* postinggen_new(char* directory, uint32_t buffersize) {
    PostingGenerator* postinggen = malloc(sizeof(PostingGenerator));
    
    postinggen->dir = string_newstr(directory);

    postinggen->bufsize = buffersize;
    postinggen->buffer = malloc(buffersize);

    postinggen->nextdocID = 0;
    postinggen->nextfilenum = 0;

    postinggen->err = 0;
}

void postinggen_addDoc(PostingGenerator* postinggen, Document doc) {

}

void postinggen_flush(PostingGenerator* postinggen) {
    
}

void postinggen_free(PostingGenerator* postinggen) {


    string_free(postinggen->dir);
    free(postinggen);
}
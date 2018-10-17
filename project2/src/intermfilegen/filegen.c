#include "filegen.h"

#include <stdlib.h>
#include <math.h>

#include "michaellib/string.h"
#include "docparser.h"
#include "michaellib/postingvec.h"
#include "michaellib/utility.h"

struct PostingGenerator {
    PostingVector* vec;

    uint32_t nextdocID;

    String* dir;
    int err;
    int nextfilenum;
};

PostingGenerator* postinggen_new(char* directory, uint32_t buffersize) {
    PostingGenerator* postinggen = malloc(sizeof(PostingGenerator));
    
    postinggen->dir = string_newstr(directory);
    
    postinggen->vec = postingvector_new(buffersize);

    postinggen->nextdocID = 0;
    postinggen->nextfilenum = 0;

    postinggen->err = 0;

    return postinggen;
}

void postinggen_addDoc(PostingGenerator* postinggen, Document doc) {
    IntermediatePostingList postinglist = docparser_getPostings(doc);

    uint32_t docID = postinggen->nextdocID;
    postinggen->nextdocID += 1;

    for(int i = 0; i < postinglist.len; i++) {
        size_t postingsize = util_getMempostingSize(
            docID,
            postinglist.head[i].freq,
            string_getLen(postinglist.head[i].term)
        );

        if(postingsize > postingvector_getBytesRemaining(postinggen->vec)) {
            postinggen_flush(postinggen);
        }

        postingvector_insert(
            postinggen->vec,
            docID,
            postinglist.head[i].freq,
            string_getString(postinglist.head[i].term)
        );
    }

    docparser_freeIntermPostingList(postinglist);
}

void postinggen_flush(PostingGenerator* postinggen) {
    // Create the filepath
    String* filepath = string_newstr(string_getString(postinggen->dir));
    string_appendString(filepath, "/", 1);

    uint32_t filenamelen = util_getDigitCount(postinggen->nextfilenum);
    char filename[filenamelen];
    snprintf(filename, filenamelen, "%d", postinggen->nextfilenum);

    string_appendString(filepath, filename, filenamelen-1);

    // Open the file and write the buffer out
    FILE* fp = fopen(string_getString(filepath), "w");
    postingvector_sortflush(postinggen->vec, fp);
    fclose(fp);

    // Increment next file number
    postinggen->nextfilenum += 1;

    string_free(filepath);
}

void postinggen_free(PostingGenerator* postinggen) {
    string_free(postinggen->dir);
    postingvector_free(postinggen->vec);
    free(postinggen);
}
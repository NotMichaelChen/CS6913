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
    // plus one for slash, getDigitCount includes \0 character
    size_t filepathlen = string_getLen(postinggen->dir) + util_getDigitCount(postinggen->nextfilenum) + 1;
    char* filepath = malloc(filepathlen);

    snprintf(filepath, filepathlen, "%s/%d", string_getString(postinggen->dir), postinggen->nextfilenum);
    
    // Open the file and write the buffer out
    FILE* fp = fopen(filepath, "w");
    postingvector_sortflush(postinggen->vec, fp);
    fclose(fp);

    // Increment next file number
    postinggen->nextfilenum += 1;

    free(filepath);
}

void postinggen_free(PostingGenerator* postinggen) {
    string_free(postinggen->dir);
    postingvector_free(postinggen->vec);
    free(postinggen);
}
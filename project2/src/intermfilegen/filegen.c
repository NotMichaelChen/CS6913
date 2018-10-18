#include "filegen.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "michaellib/string.h"
#include "docparser.h"
#include "michaellib/buffer.h"
#include "michaellib/utility.h"

void writePostingList(MemPosting* postinglist, size_t len, FILE* fp) {
    for(size_t i = 0; i < len; i++) {
        fprintf(fp, "%s %lu %lu\n",
            string_getString(postinglist[i].term),
            postinglist[i].docID,
            postinglist[i].freq
        );
    }

    for(size_t i = 0; i < len; i++) {
        string_free(postinglist[i].term);
    }

    free(postinglist);
}

struct PostingGenerator {
    Buffer* buf;

    uint32_t nextdocID;

    String* dir;
    int err;
    int nextfilenum;
};

PostingGenerator* postinggen_new(char* directory, uint32_t buffersize) {
    PostingGenerator* postinggen = malloc(sizeof(PostingGenerator));
    
    
    postinggen->dir = string_newstr(directory);
    
    postinggen->buf = buffer_new(buffersize);

    postinggen->nextdocID = 0;
    postinggen->nextfilenum = 0;

    postinggen->err = 0;

    return postinggen;
}

void postinggen_addDoc(PostingGenerator* postinggen, Document doc) {
    IntermediatePostingList postinglist = docparser_getPostings(doc);

    size_t docID = postinggen->nextdocID;
    postinggen->nextdocID += 1;

    for(int i = 0; i < postinglist.len; i++) {
        size_t totalsize = sizeof(size_t) * 2 + string_getLen(postinglist.head[i].term) + 1;
        char* line = malloc(totalsize);
        //Copy the null terminator too
        memcpy(line, string_getString(postinglist.head[i].term), string_getLen(postinglist.head[i].term) + 1);
        char* nextloc = line + string_getLen(postinglist.head[i].term) + 1;
        memcpy(nextloc, &docID, sizeof(docID));
        nextloc += sizeof(docID);
        memcpy(nextloc, &(postinglist.head[i].freq), sizeof(postinglist.head[i].freq));

        if(totalsize > buffer_getRemaining(postinggen->buf)) {
            postinggen_flush(postinggen);
        }

        buffer_write(postinggen->buf, line, totalsize);

        free(line);
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
    size_t postinglistlen = 0;
    MemPosting* postinglist = buffer_getPostings(postinggen->buf, &postinglistlen);
    qsort(postinglist, postinglistlen, sizeof(MemPosting), memposting_cmp);

    writePostingList(postinglist, postinglistlen, fp);

    fclose(fp);

    // Increment next file number
    postinggen->nextfilenum += 1;

    free(filepath);
}

void postinggen_free(PostingGenerator* postinggen) {
    string_free(postinggen->dir);
    buffer_free(postinggen->buf);
    free(postinggen);
}
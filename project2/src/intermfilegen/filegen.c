#include "filegen.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "michaellib/string.h"
#include "docparser.h"
#include "michaellib/buffer.h"
#include "michaellib/utility.h"

// Private utility function that writes a list of postings to disk. Assumes the
// list is already sorted. Note that these postings also contain the docID
void writePostingList(MemPosting* postinglist, size_t len, FILE* fp) {
    for(size_t i = 0; i < len; i++) {
        // Each posting is written out "term docID freq" with spaces and newline
        fprintf(fp, "%s %lu %lu\n",
            string_getString(postinglist[i].term),
            postinglist[i].docID,
            postinglist[i].freq
        );
    }

    // Frees the posting list once it's done writing it out
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
    // Generate the intermediate posting list from the document
    IntermediatePostingList postinglist = docparser_getPostings(doc);

    // Get the next available docID
    size_t docID = postinggen->nextdocID;
    postinggen->nextdocID += 1;

    for(int i = 0; i < postinglist.len; i++) {
        // Represents how much space the byte-representation of a
        // term-docID-freq posting would take
        // Two size_t ints for docID + freq, then the length of the term + null
        // terminating character
        size_t totalsize = sizeof(size_t) * 2 + string_getLen(postinglist.head[i].term) + 1;
        // Allocate the byte array
        char* line = malloc(totalsize);
        
        // Copy the string into the byte array. Copy the null terminator too
        memcpy(line, string_getString(postinglist.head[i].term), string_getLen(postinglist.head[i].term) + 1);

        // Find out where to write the docID. Should be right after the term's
        // null terminator
        char* nextloc = line + string_getLen(postinglist.head[i].term) + 1;
        // Copy the docID there
        memcpy(nextloc, &docID, sizeof(docID));
        
        // Find out where to write the freq by skipping over how big the docID
        // is
        nextloc += sizeof(docID);
        // Copy the frequency there
        memcpy(nextloc, &(postinglist.head[i].freq), sizeof(postinglist.head[i].freq));

        // If there's not enough space to save this posting in the buffer, flush
        // it out
        if(totalsize > buffer_getRemaining(postinggen->buf)) {
            postinggen_flush(postinggen);
        }

        // Add the posting to the buffer
        buffer_write(postinggen->buf, line, totalsize);

        free(line);
    }

    // Free the posting list once we're done with it
    docparser_freeIntermPostingList(postinglist);
}

void postinggen_flush(PostingGenerator* postinggen) {
    // Create the filepath
    // plus one for slash, since getDigitCount includes \0 character
    size_t filepathlen = string_getLen(postinggen->dir) + util_getDigitCount(postinggen->nextfilenum) + 1;
    char* filepath = malloc(filepathlen);

    snprintf(filepath, filepathlen, "%s/%d", string_getString(postinggen->dir), postinggen->nextfilenum);
    
    // Open the file
    FILE* fp = fopen(filepath, "w");

    // Get a list of term-docID-freq postings from the buffer
    size_t postinglistlen = 0;
    MemPosting* postinglist = buffer_getPostings(postinggen->buf, &postinglistlen);

    // qsort the list of postings
    qsort(postinglist, postinglistlen, sizeof(MemPosting), memposting_cmp);

    // Write out that posting list
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
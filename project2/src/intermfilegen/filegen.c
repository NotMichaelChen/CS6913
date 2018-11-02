#include "filegen.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "michaellib/string.h"
#include "docparser.h"
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

    // Frees each string in the posting list once it's done writing it out
    for(size_t i = 0; i < len; i++) {
        string_free(postinglist[i].term);
    }
}

struct PostingGenerator {
    MemPosting* postinglist;
    uint32_t listindex;
    uint32_t listsize;

    docID_t nextdocID;

    String* dir;
    int err;
    int nextfilenum;
};

PostingGenerator* postinggen_new(char* directory, uint32_t buffersize) {
    PostingGenerator* postinggen = malloc(sizeof(PostingGenerator));
    
    postinggen->dir = string_newstr(directory);
    postinggen->postinglist = malloc(sizeof(MemPosting) * buffersize);
    postinggen->listsize = buffersize;
    postinggen->listindex = 0;

    postinggen->nextdocID = 0;
    postinggen->nextfilenum = 0;

    postinggen->err = 0;

    return postinggen;
}

void postinggen_addDoc(PostingGenerator* postinggen, Document doc) {
    // Get the next available docID
    docID_t docID = postinggen->nextdocID;
    postinggen->nextdocID += 1;

    // Generate the intermediate posting list from the document
    MemPostingList postinglist = docparser_getPostings(doc, docID);

    // Write out the posting list buffer if there's no more room
    if(postinggen->listindex + postinglist.len > postinggen->listsize) {
        postinggen_flush(postinggen);
    }

    // Copy the postinglist into the buffer
    memcpy(&postinggen->postinglist[postinggen->listindex], postinglist.head, sizeof(MemPosting) * postinglist.len);
    postinggen->listindex += postinglist.len;

    // Just free the posting buffer, not the actual strings inside the postings
    // Those are still referenced in the postinglist buffer
    free(postinglist.head);
}

void postinggen_flush(PostingGenerator* postinggen) {
    // Create the filepath
    // plus one for slash, since getDigitCount includes \0 character
    size_t filepathlen = string_getLen(postinggen->dir) + util_getDigitCount(postinggen->nextfilenum) + 1;
    char* filepath = malloc(filepathlen);

    snprintf(filepath, filepathlen, "%s/%d", string_getString(postinggen->dir), postinggen->nextfilenum);
    
    // Open the file
    FILE* fp = fopen(filepath, "w");

    // qsort the list of postings
    qsort(postinggen->postinglist, postinggen->listindex, sizeof(MemPosting), memposting_cmp);

    // Write out that posting list
    writePostingList(postinggen->postinglist, postinggen->listindex, fp);

    // No need to free posting list: writePostingList does that for us
    postinggen->listindex = 0;

    fclose(fp);

    // Increment next file number
    postinggen->nextfilenum += 1;

    free(filepath);
}

void postinggen_free(PostingGenerator* postinggen) {
    string_free(postinggen->dir);
    free(postinggen->postinglist);
    free(postinggen);
}
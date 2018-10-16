#include "filegen.h"

#include <stdlib.h>
#include <math.h>

#include "michaellib/string.h"
#include "michaellib/buffer.h"
#include "docparser.h"

#define NUMBER_ASCII_LENGTH(X) (X == 0) ? 1 : (sizeof(char)*(int)log10(X))+2

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
    //To avoid issues with NUMBER_ASCII_LENGTH
    postinggen->nextfilenum = 1;

    postinggen->err = 0;
}

void postinggen_addDoc(PostingGenerator* postinggen, Document doc) {
    IntermediatePostingList postinglist = docparser_getPostings(doc);

    uint32_t docID = postinggen->nextdocID;
    postinggen->nextdocID += 1;

    for(int i = 0; i < postinglist.len; i++) {
        String* line = string_newstr(string_getString(postinglist.head[i].term));
        string_appendString(line, " ", 1);

        //TODO: avoid computing this every time
        uint32_t docIDlen = NUMBER_ASCII_LENGTH(docID);
        char docIDstr[docIDlen];
        snprintf(docIDstr, docIDlen, "%d", docID);

        uint32_t freq = postinglist.head[i].freq;
        uint32_t freqlen = NUMBER_ASCII_LENGTH(freq);
        char freqstr[freqlen];
        snprintf(freq, freqlen, "%d", freq);

        string_appendString(line, docIDstr, docIDlen-1);
        string_appendString(line, " ", 1);
        string_appendString(line, freqstr, freqlen-1);
        string_appendString(line, "\n", 1);

        size_t linelen = string_getLen(line);
        if(linelen > buffer_getRemaining(postinggen->buf)) {
            postinggen_flush(postinggen);
        }

        buffer_write(postinggen->buf, string_getString(line), linelen);

        string_free(line);
    }

    free(postinglist.head);
}

void postinggen_flush(PostingGenerator* postinggen) {
    // Create the filepath
    String* filepath = string_newstr(string_getString(postinggen->dir));
    string_appendString(filepath, "/", 1);

    uint32_t filenamelen = NUMBER_ASCII_LENGTH(postinggen->nextfilenum);
    char filename[filenamelen];
    snprintf(filename, filenamelen, "%d", postinggen->nextfilenum);

    string_appendString(filepath, filename, filenamelen-1);

    // Open the file and write the buffer out
    FILE* fp = fopen(string_getString(filepath), "w");
    buffer_flush(postinggen->buf, fp);
    fclose(fp);

    // Increment next file number
    postinggen->nextfilenum += 1;

    string_free(filepath);
}

void postinggen_free(PostingGenerator* postinggen) {
    buffer_free(postinggen->buf);
    string_free(postinggen->dir);
    free(postinggen);
}
#include "docparser.h"

#include "lib/uthash.h"

MemPostingList docparser_getPostings(Document doc, docID_t docID) {

    // Represent an entry in the term-frequency dictionary
    struct frequency {
        char* term;
        freq_t freq;
        UT_hash_handle hh;
    };

    // Construct an instance of the term-frequency dictionary
    struct frequency* freqdict = NULL;
    // Represents how many terms were inserted into the dictionary. Will be used
    // later to allocated the final posting list
    size_t insertioncount = 0;

    // Make a temporary copy of the document, since we're about to modify it by
    // tokenizing it
    char* tempdoc = malloc(doc.docsize + 1);
    strcpy(tempdoc, doc.doc);

    // Begin tokenizing on whitespace
    char* docwalker = strtok(tempdoc, " \r\n\t");

    // Continue tokenizing until there's no more tokens
    size_t tokencount = 0;
    while(docwalker != NULL) {
        tokencount++;
        // Get the length of the token/term
        size_t docwalkerlen = strlen(docwalker);

        // Attempt to find an entry that matches the found term
        struct frequency* f;
        HASH_FIND_STR(freqdict, docwalker, f);

        // If no entry is found, construct a new entry and insert it
        if(f == NULL) {
            // Create the entry and copy the term into it
            f = malloc(sizeof(struct frequency));
            f->term = malloc(docwalkerlen+1);
            strcpy(f->term, docwalker);
            // Set the frequency to 1
            f->freq = 1;
            // Add the entry to the dictionary
            HASH_ADD_KEYPTR(hh, freqdict, f->term, docwalkerlen, f);
            ++insertioncount;
        }
        else {
            // To update, the entry must be removed from the dictionary,
            // updated, then reinserted
            HASH_DEL(freqdict, f);
            f->freq += 1;
            HASH_ADD_KEYPTR(hh, freqdict, f->term, docwalkerlen, f);
        }

        // Tokenize again
        docwalker = strtok(NULL, " \r\n\t");
    }

    // Construct the postinglist and allocate enough space to handle all of the
    // terms
    MemPostingList postinglist;
    postinglist.len = insertioncount;
    postinglist.head = malloc(sizeof(MemPosting) * insertioncount);
    postinglist.termcount = tokencount;

    struct frequency* f = NULL;
    struct frequency* tmp = NULL;
    size_t insertindex = 0;
    // Iterate over every entry in the dictionary
    HASH_ITER(hh, freqdict, f, tmp) {
        // Make a copy of the term in the entry
        String* str = string_newstr(f->term);
        // Insert a new posting in the next available position
        postinglist.head[insertindex] = (MemPosting) {str, docID, f->freq};

        // Remove the entry from the dictionary
        HASH_DEL(freqdict, f);

        // Free the entry
        free(f->term);
        free(f);

        insertindex++;
    }

    free(tempdoc);

    return postinglist;
}

void docparser_freePostingList(MemPostingList postinglist) {
    // Iterate over every posting and free its term
    for(size_t i = 0; i < postinglist.len; i++) {
        string_free(postinglist.head[i].term);
    }
    free(postinglist.head);
}
#include "docparser.h"

#include "lib/uthash.h"

IntermediatePostingList docparser_getPostings(Document doc) {
    
    // Represent an entry in the term-frequency dictionary
    struct frequency {
        char* term;
        uint32_t freq;
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
    while(docwalker != NULL) {
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
    IntermediatePostingList postinglist;
    postinglist.len = insertioncount;
    postinglist.head = malloc(sizeof(IntermediatePosting) * insertioncount);

    struct frequency* f;
    struct frequency* tmp;
    size_t insertindex = 0;
    // Iterate over every entry in the dictionary
    HASH_ITER(hh, freqdict, f, tmp) {
        // Make a copy of the term in the entry
        String* str = string_newstr(f->term);
        // Insert a new posting in the next available position
        postinglist.head[insertindex] = (IntermediatePosting) {str, f->freq};

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

void docparser_freeIntermPostingList(IntermediatePostingList postinglist) {
    // Iterate over every posting and free its term
    for(size_t i = 0; i < postinglist.len; i++) {
        string_free(postinglist.head[i].term);
    }
    free(postinglist.head);
}
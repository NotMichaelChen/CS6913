#include "docparser.h"

#include "lib/uthash.h"

IntermediatePostingList docparser_getPostings(Document doc) {
    
    struct frequency {
        char* term;
        uint32_t freq;
        UT_hash_handle hh;
    };

    struct frequency* freqdict = NULL;
    size_t insertioncount = 0;

    char* tempdoc = malloc(doc.docsize + 1);
    strcpy(tempdoc, doc.doc);

    char* docwalker = strtok(tempdoc, " \r\n\t");
    
    while(docwalker != NULL) {
        size_t docwalkerlen = strlen(docwalker);

        struct frequency* f;
        HASH_FIND_STR(freqdict, docwalker, f);

        if(f == NULL) {
            f = malloc(sizeof(struct frequency));
            f->term = malloc(docwalkerlen+1);
            strcpy(f->term, docwalker);
            f->freq = 1;
            HASH_ADD_KEYPTR(hh, freqdict, f->term, docwalkerlen, f);
            ++insertioncount;
        }
        else {
            HASH_DEL(freqdict, f);
            f->freq += 1;
            HASH_ADD_KEYPTR(hh, freqdict, f->term, docwalkerlen, f);
        }

        docwalker = strtok(NULL, " \r\n\t");
    }

    IntermediatePostingList postinglist;
    postinglist.len = insertioncount;
    postinglist.head = malloc(sizeof(IntermediatePosting) * insertioncount);

    struct frequency* f;
    struct frequency* tmp;
    size_t insertindex = 0;
    HASH_ITER(hh, freqdict, f, tmp) {
        String* str = string_newstr(f->term);
        postinglist.head[insertindex] = (IntermediatePosting) {str, f->freq};

        HASH_DEL(freqdict, f);

        free(f->term);
        free(f);

        insertindex++;
    }

    free(tempdoc);

    return postinglist;
}

void docparser_freeIntermPostingList(IntermediatePostingList postinglist) {
    for(size_t i = 0; i < postinglist.len; i++) {
        string_free(postinglist.head[i].term);
    }
    free(postinglist.head);
}
#include "docwriter.h"

#include "posting.h"

typedef struct IntermediatePostingList {
    IntermediatePosting* head;
    size_t len;
} IntermediatePostingList;

IntermediatePostingList docwriter_getPostings(Document doc) {
    
    struct frequency {
        char* term;
        size_t freq;
        UT_hash_handle hh;
    };

    struct frequency* freqdict = NULL;
    size_t insertioncount = 0;

    char* tempdoc = malloc(doc.docsize + 1);
    strncpy(tempdoc, doc.doc, doc.docsize + 1);

    char* docwalker = strtok(tempdoc, " \r\n\t");
    
    while(docwalker != NULL) {
        size_t docwalkerlen = strlen(docwalker);

        struct frequency* f;
        HASH_FIND_STR(freqdict, docwalker, f);

        if(f == NULL) {
            f = malloc(sizeof(struct frequency));
            f->term = malloc(docwalkerlen+1);
            strncpy(f->term, docwalker, docwalkerlen+1);
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

        free(f->term);
        free(f);

        insertindex++;
    }

    free(tempdoc);

    return postinglist;
}

void docwriter_writePostings(IntermediatePostingList postinglist, size_t docID, FILE* fp, LexiconEntry* lexent) {
    for(size_t i = 0; i < postinglist.len; ++i) {
        //Write current position to lexicon
        LexiconEntry* l;
        HASH_FIND_STR(lexent, string_getString(postinglist.head[i].term), l);

        if(l == NULL) {
            size_t strlen = string_getLen(postinglist.head[i].term);
            l = malloc(sizeof(LexiconEntry));
            l->term = malloc(strlen + 1);
            strncpy(l->term, string_getString(postinglist.head[i].term), strlen+1);
            l->pos = ftell(fp);
            HASH_ADD_KEYPTR(hh, lexent, l->term, strlen, l);
        }
        else {
            HASH_DEL(lexent, l);
            l->pos = ftell(fp);
            HASH_ADD_KEYPTR(hh, lexent, l->term, string_getLen(postinglist.head[i].term), l);
        }

        //Write docid, then frequency
        fwrite(&docID, sizeof(docID), 1, fp);
        fwrite(&postinglist.head[i].freq, sizeof(postinglist.head[i].freq), 1, fp);
    }
}

void docwriter_writeDoc(Document doc, size_t docID, FILE* fp, LexiconEntry* lexent) {
    docwriter_writePostings(docwriter_getPostings(doc), docID, fp, lexent);
}
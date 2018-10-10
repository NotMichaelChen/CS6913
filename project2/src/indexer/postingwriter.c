#include "postingwriter.h"

void writePostings(IntermediatePostingList postinglist, uint32_t docID, FILE* fp, LexiconEntry* lexent) {
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
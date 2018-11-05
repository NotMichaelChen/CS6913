#include "utility.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

inline int util_getDigitCount(size_t num) {
    return (num == 0) ? 2 : (sizeof(char)*(int)log10(num))+2;
}

void util_printSnippet(char* doc, char* term) {
    size_t doclen = strlen(doc);
    size_t termlen = strlen(term);

    size_t termloc = 0;
    size_t snippetbegin = 0;
    int snippetlen = 0;

    while(termloc <= doclen - termlen) {
        //Equal
        if(!strncmp(doc + termloc, term, termlen))
            break;

        termloc++;
    }

    //TODO: make constant

    snippetbegin = termloc >= 500 ? termloc - 500 : 0;
    snippetlen = doclen - snippetbegin >= 1000 ? 1000 : doclen - snippetbegin;

    for(int i = snippetbegin; i < snippetlen; i++) {
        if(doc[i] == '\n' || doc[i] == '\r')
            doc[i] = ' ';
    }

    printf("%.*s\n\n", snippetlen, doc + snippetbegin);
}

// Comparison functions

int util_gcmpHeapEntry(const void* a, const void* b) {
    const HeapEntry *c = b, *d = a;

    return (c->score > d->score) - (c->score < d->score);
}
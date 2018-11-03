#include "DAAT.h"

#include "listpointer.h"

void DAAT(char** terms, size_t termcount, Lexicon* lex, FILE* fp) {
    ListPointer** lps = malloc(sizeof (ListPointer*) * termcount);

    for(size_t i = 0; i < termcount; i++) {
        lps[i] = listpointer_open(terms[i], lex, fp);
    }

    docID_t docID;
    bool success = true;
    while(1) {
        docID = listpointer_nextGEQ(lps[0], docID, &success);
        if(!success) break;

        docID_t otherdocID;
        for(size_t i = 1; i < termcount; i++) {
            otherdocID = listpointer_nextGEQ(lps[i], docID, &success);
            if(otherdocID != docID) {
                break;
            }
        }
        if(!success) break;

        // Not in intersection
        if(otherdocID > docID) {
            docID = otherdocID;
        }
        // docID is in intersection
        else {
            freq_t* frequencies = malloc(sizeof (freq_t) * termcount);
            for(size_t i = 0; i < termcount; i++) {
                frequencies[i] = listpointer_getFreq(lps[i]);
            }

            // Compute BM25

            free(frequencies);
            docID++;
        }
    }

    free(lps);
}
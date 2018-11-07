#include "DAAT.h"

#include "BM25.h"
#include "listpointer.h"

MinHeap* DAAT(char** terms, size_t termcount, Lexicon* lex, PageTable* pagetable, FILE* fp) {
    if(termcount == 0) {
        return minheap_new(10);
    }

    //Construct list that determines how many docs contain each term
    uint32_t* docscontaining = malloc(sizeof (uint32_t) * termcount);
    //Construct list pointers
    ListPointer** lps = malloc(sizeof (ListPointer*) * termcount);

    MinHeap* heap = minheap_new(10);
    
    //Fill lists
    for(size_t i = 0; i < termcount; i++) {
        lps[i] = listpointer_open(terms[i], lex, fp);
        docscontaining[i] = lexicon_getlistlen(lex, terms[i]);
    }

    docID_t docID = 0;
    bool success = true;
    while(1) {
        docID = listpointer_nextGEQ(lps[0], docID, &success);
        if(!success) break;

        docID_t otherdocID = docID;
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
            double score = BM25(frequencies, termcount, docscontaining, termcount,
                pagetable_getTermCount(pagetable, docID),
                pagetable_getAvgTermCount(pagetable),
                pagetable_len(pagetable)
            );

            minheap_attemptInsert(heap, (HeapEntry) {score, docID});

            free(frequencies);
            docID++;
        }
    }

    for(size_t i = 0; i < termcount; i++) {
        listpointer_close(lps[i]);
    }

    free(lps);
    free(docscontaining);

    return heap;
}
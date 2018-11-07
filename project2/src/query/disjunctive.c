#include "disjunctive.h"

#include "BM25.h"
#include "listpointer.h"

MinHeap* disjunctive_query(char** terms, size_t termcount, Lexicon* lex, PageTable* pagetable, String* indexpath) {
    if(termcount == 0) {
        return minheap_new(10);
    }

    //Construct list that determines how many docs contain each term
    uint32_t* docscontaining = malloc(sizeof (uint32_t) * termcount);
    //Construct list pointers
    ListPointer** lps = malloc(sizeof (ListPointer*) * termcount);
    //Construct a list of all file pointers
    FILE** fps = malloc(sizeof (FILE*) * termcount);

    MinHeap* heap = minheap_new(10);
    
    //Fill lists
    for(size_t i = 0; i < termcount; i++) {
        fps[i] = fopen(string_getString(indexpath), "rb");
        lps[i] = listpointer_open(terms[i], lex, fps[i]);
        docscontaining[i] = lexicon_getlistlen(lex, terms[i]);
    }

    docID_t docID = 0;
    bool success = true;
    docID_t* lpdocID = malloc(sizeof (docID_t) * termcount);
    while(1) {

        //Call nextGEQ for each list pointer - keep track of the minimum docID
        docID_t mindocID = listpointer_nextGEQ(lps[0], docID, &success);
        if(!success) break;
        lpdocID[0] = mindocID;

        for(size_t i = 1; i < termcount; i++) {
            lpdocID[i] = listpointer_nextGEQ(lps[i], docID, &success);
            if(!success) break;
            if(lpdocID[i] < mindocID)
                mindocID = lpdocID[i];
        }
        if(!success) break;

        docID = mindocID;

        //Only get the frequencies of listpointers of the minimum docID
        //otherwise set the freq to zero
        freq_t* frequencies = malloc(sizeof (freq_t) * termcount);
        for(size_t i = 0; i < termcount; i++) {
            if(lpdocID[i] == docID)
                frequencies[i] = listpointer_getFreq(lps[i]);
            else
                frequencies[i] = 0;
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

    for(size_t i = 0; i < termcount; i++) {
        fclose(fps[i]);
        listpointer_close(lps[i]);
    }

    free(lpdocID);
    free(lps);
    free(fps);
    free(docscontaining);

    return heap;
}
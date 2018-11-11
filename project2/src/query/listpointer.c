#include "listpointer.h"

#include <string.h>

#include "compression/varbyte.h"

struct ListPointer {
    FILE* fp;

    //metadata
    uint64_t* lastdocid;
    size_t lastdocidlen;
    uint64_t* blocksizes;
    size_t blocksizeslen;

    //decompressed blocks
    //TODO: make length global param
    ULongVector* docIDblock;
    ULongVector* freqblock;

    //indexes
    //Index within block
    size_t docIDindex;
    //Index of blocks
    size_t blockindex;

    //other
    size_t blockposition;
    bool freqavailable;
    bool isValid;
};

ListPointer* listpointer_open(char* term, Lexicon* lex, FILE* fp) {
    ListPointer* lp = calloc(1, sizeof (ListPointer));
    lp->isValid = true;
    lp->fp = fp;

    // Get lexicon data
    size_t filepos = lexicon_getpos(lex, term);
    size_t metalen = lexicon_getmetasize(lex, term);

    // Seek to posting list and read metadata
    fseek(fp, filepos, SEEK_SET);
    uint8_t* metacompr = malloc(metalen);
    size_t metaread = fread(metacompr, 1, metalen, fp);
    if(metaread != metalen) {
        lp->isValid = false;
        return lp;
    }

    // Decompress metadata
    ULongVector* metadatavec = varbyte_decodeStream(metacompr, metalen);
    lp->blocksizeslen = ulongvector_get(metadatavec, 0);
    //skip blocksizeslen numbers and account for original len
    lp->lastdocidlen = ulongvector_get(metadatavec, lp->blocksizeslen + 1);

    lp->blocksizes = malloc(sizeof (uint64_t) * lp->blocksizeslen);
    lp->lastdocid = malloc(sizeof (uint64_t) * lp->lastdocidlen);

    uint64_t* rawmetadata = ulongvector_getBuffer(metadatavec);
    // Constants 1 and 2 are for accounting for the actual len entries
    memcpy(lp->blocksizes, rawmetadata + 1, lp->blocksizeslen * sizeof (*rawmetadata));
    memcpy(lp->lastdocid, rawmetadata + lp->blocksizeslen + 2, lp->lastdocidlen * sizeof (*rawmetadata));

    // Decompress and store first docID
    lp->blockposition = ftell(fp);

    uint8_t* blockcompr = malloc(lp->blocksizes[0]);
    size_t blockread = fread(blockcompr, 1, lp->blocksizes[0], fp);
    if(blockread != lp->blocksizes[0]) {
        lp->isValid = false;
        return lp;
    }

    lp->docIDblock = varbyte_decodeStream(blockcompr, lp->blocksizes[0]);
    lp->freqblock = NULL;

    lp->docIDindex = 0;
    lp->blockindex = 0;
    lp->freqavailable = false;

    // Free stuff
    ulongvector_free(metadatavec);
    free(metacompr);
    free(blockcompr);

    return lp;
}

docID_t listpointer_nextGEQ(ListPointer* lp, docID_t docID, bool* success) {
    *success = true;

    if(!lp->isValid) {
        *success = false;
        return -1;
    }

    size_t oldblockindex = lp->blockindex;
    //Find the correct block to look at
    while(lp->blockindex < lp->lastdocidlen && lp->lastdocid[lp->blockindex] < docID) {
        //Move the docID block pointer
        lp->blockposition += lp->blocksizes[(lp->blockindex*2)] + lp->blocksizes[(lp->blockindex*2)+1];

        ++lp->blockindex;
    }
    if(lp->blockindex == lp->lastdocidlen) {
        lp->isValid = false;
        *success = false;
        return -1;
    }

    //If it's different than our current block then decompress new block
    if(oldblockindex != lp->blockindex) {
        //Move the filestream to the new position
        fseek(lp->fp, lp->blockposition, SEEK_SET);
        size_t buffersize = lp->blocksizes[lp->blockindex*2];

        uint8_t* blockcompr = malloc(buffersize);
        size_t blockread = fread(blockcompr, 1, buffersize, lp->fp);
        if(blockread != buffersize) {
            fprintf(stderr,
                "Error: docID block %lu does not match expected size. %lu read vs %lu expected.\n",
                lp->blockindex,
                blockread,
                buffersize
            );
            exit(1);
        }

        if(lp->docIDblock != NULL)
            ulongvector_free(lp->docIDblock);
        lp->docIDblock = varbyte_decodeStream(blockcompr, lp->blocksizes[0]);


        lp->docIDindex = 0;
        lp->freqavailable = false;
    }
    //Perform standard docID searching
    while(lp->docIDindex < ulongvector_size(lp->docIDblock) && ulongvector_get(lp->docIDblock, lp->docIDindex) < docID)
        ++lp->docIDindex;

    if(lp->docIDindex == ulongvector_size(lp->docIDblock)) {
        lp->isValid = false;
        *success = false;
        return -1;
    }
    
    return ulongvector_get(lp->docIDblock, lp->docIDindex);
}

freq_t listpointer_getFreq(ListPointer* lp) {
    if(!lp->freqavailable) {
        if(lp->freqblock != NULL)
            ulongvector_free(lp->freqblock);

        uint8_t* blockcompr = malloc(lp->blocksizes[(lp->blockindex*2)+1]);
        size_t blockread = fread(blockcompr, 1, lp->blocksizes[(lp->blockindex*2)+1], lp->fp);
        if(blockread != lp->blocksizes[(lp->blockindex*2)+1]) {
            fprintf(stderr,
                "Error: frequency block %lu does not match expected size. %lu read vs %lu expected.\n",
                lp->blockindex,
                blockread,
                lp->blocksizes[(lp->blockindex*2)+1]
            );
            exit(1);
        }

        lp->freqblock = varbyte_decodeStream(blockcompr, lp->blocksizes[(lp->blockindex*2)+1]);
        free(blockcompr);
        lp->freqavailable = true;
    }

    if(lp->docIDindex >= ulongvector_size(lp->freqblock)) {
        fprintf(stderr,
            "Error: frequency block %lu not large enough (size: %lu, index: %lu, docIDsize: %lu).\n",
            lp->blockindex,
            ulongvector_size(lp->freqblock),
            lp->docIDindex,
            ulongvector_size(lp->docIDblock)
        );
        exit(1);
    }
    return ulongvector_get(lp->freqblock, lp->docIDindex);
}

void listpointer_close(ListPointer* lp) {
    free(lp->lastdocid);
    free(lp->blocksizes);

    if(lp->docIDblock != NULL)
        ulongvector_free(lp->docIDblock);
    if(lp->freqblock != NULL)
        ulongvector_free(lp->freqblock);

    free(lp);
}
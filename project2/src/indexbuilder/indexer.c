#define _XOPEN_SOURCE 700
#include "indexbuilder/indexer.h"

#include <string.h>
#include <stdio.h>

#include "global.h"
#include "michaellib/string.h"
#include "michaellib/vector/ulongvec.h"
#include "michaellib/vector/bytevec.h"
#include "compression/varbyte.h"

// Write a vector of numbers to a file as a block-based posting list
void writePostingData(FILE* fp, Lexicon* lex, ULongVector* vec, String* term) {
    if(ulongvector_size(vec) % 2 != 0) {
        fprintf(stderr, "Error: vec of numbers to write as posting list not even\n");
        exit(1);
    }

    ULongVector* blocksizes = ulongvector_new();
    ULongVector* lastdocid = ulongvector_new();

    ByteVec* postingdata = bytevec_new();

    //TODO: make 128 constant
    uint64_t docIDblock[128];
    uint64_t freqblock[128];
    size_t blockindex = 0;

    for(size_t i = 0; i < ulongvector_size(vec); i += 2) {
        docIDblock[blockindex] = ulongvector_get(vec, i);
        freqblock[blockindex] = ulongvector_get(vec, i+1);

        blockindex++;

        //TODO: make constant
        if(blockindex == 128) {
            //Add to lastdocid
            ulongvector_append(lastdocid, docIDblock[blockindex-1]);

            //Compress docIDs
            ByteVec* comprblock = varbyte_encodeblock(docIDblock, 128);
            bytevec_concat(postingdata, comprblock);
            ulongvector_append(blocksizes, bytevec_len(comprblock));
            bytevec_free(comprblock);

            //Compress freqs
            comprblock = varbyte_encodeblock(freqblock, 128);
            bytevec_concat(postingdata, comprblock);
            ulongvector_append(blocksizes, bytevec_len(comprblock));
            bytevec_free(comprblock);

            blockindex = 0;
        }
    }

    //Write out last two blocks if they have stuff in them
    if(blockindex != 0) {
        //Add to lastdocid
        ulongvector_append(lastdocid, docIDblock[blockindex-1]);

        //Compress docIDs
        ByteVec* comprblock = varbyte_encodeblock(docIDblock, blockindex);
        bytevec_concat(postingdata, comprblock);
        ulongvector_append(blocksizes, bytevec_len(comprblock));
        bytevec_free(comprblock);

        //Compress freqs
        comprblock = varbyte_encodeblock(freqblock, blockindex);
        bytevec_concat(postingdata, comprblock);
        ulongvector_append(blocksizes, bytevec_len(comprblock));
        bytevec_free(comprblock);
    }

    //Compute metadata
    ByteVec* compressedblocksizes = varbyte_encodeblock(ulongvector_getBuffer(blocksizes), ulongvector_size(blocksizes));
    ByteVec* compressedlastdocid = varbyte_encodeblock(ulongvector_getBuffer(lastdocid), ulongvector_size(lastdocid));

    uint8_t* blocksizeslen;
    size_t blocksizeslenlen = 0;
    uint8_t* lastdocidlen;
    size_t lastdocidlenlen = 0;

    blocksizeslen = varbyte_encode(ulongvector_size(blocksizes), &blocksizeslenlen);
    lastdocidlen = varbyte_encode(ulongvector_size(lastdocid), &lastdocidlenlen);

    size_t metasize = blocksizeslenlen + lastdocidlenlen +
                        bytevec_len(compressedblocksizes) + bytevec_len(compressedlastdocid);
    
    //Add lexicon entry
    lexicon_insert(lex, string_getString(term), string_getLen(term), ftell(fp), metasize, ulongvector_size(vec) / 2);

    //Write everything out
    fwrite(blocksizeslen, 1, blocksizeslenlen, fp);
    bytevec_dump(compressedblocksizes, fp);
    fwrite(lastdocidlen, 1, lastdocidlenlen, fp);
    bytevec_dump(compressedlastdocid, fp);
    bytevec_dump(postingdata, fp);

    //Free everything
    ulongvector_free(blocksizes);
    ulongvector_free(lastdocid);
    bytevec_free(postingdata);
    bytevec_free(compressedblocksizes);
    bytevec_free(compressedlastdocid);
    free(blocksizeslen);
    free(lastdocidlen);
}

Lexicon* block_generateIndex(FILE* ifp, FILE* ofp) {
    Lexicon* lex = lexicon_new();

    String* currentterm = string_newstr("");
    // Since our posting list is just a list of <docID,freq> pairs, we can
    // represent that as a list of numbers
    ULongVector* postinglist = ulongvector_new();

    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    // Read through the intermediate file line by line
    while ((read = getline(&line, &linelen, ifp)) != -1) {
        // Represents the pointer used to tokenize the line
        char* linewalker = strtok(line, "\t\r\n ");
        if(linewalker == NULL)
            return NULL;
        
        // New term not equal to old term
        if(strcmp(linewalker, string_getString(currentterm))) {
            // Special case for first word, simply set the word without writing
            // out anything
            if(!strcmp("", string_getString(currentterm))) {
                string_free(currentterm);
                currentterm = string_newstr(linewalker);
            }
            else {
                writePostingData(ofp, lex, postinglist, currentterm);

                // Empty posting list
                ulongvector_clear(postinglist);

                // Get new term
                string_free(currentterm);
                currentterm = string_newstr(linewalker);
            }
        }

        // Regardless of what happened, need to store new numbers
        linewalker = strtok(NULL, "\t\r\n ");
        // docID
        ulongvector_append(postinglist, strtoull(linewalker, NULL, 10));
        linewalker = strtok(NULL, "\t\r\n ");
        // frequency
        ulongvector_append(postinglist, strtoull(linewalker, NULL, 10));
    }

    // Write out rest of posting list
    if(ulongvector_size(postinglist) != 0)
        writePostingData(ofp, lex, postinglist, currentterm);

    free(line);
    string_free(currentterm);
    ulongvector_free(postinglist);

    return lex;
}

Lexicon* block_buildIndex(char* dir, char* input, char* output) {
    size_t dirlen = strlen(dir);
    size_t inputlen = strlen(input);
    size_t outputlen = strlen(output);

    // Construct the input filepath
    char* inputfilename = malloc(dirlen + inputlen + 2);
    snprintf(inputfilename, dirlen+inputlen+2, "%s/%s", dir, input);

    // Construct the output filepath
    char* outputfilename = malloc(dirlen + outputlen + 2);
    snprintf(outputfilename, dirlen+outputlen+2, "%s/%s", dir, output);

    // Open the files
    FILE* ifp = fopen(inputfilename, "r");
    FILE* ofp = fopen(outputfilename, "w");

    // Generate the index
    Lexicon* lex = block_generateIndex(ifp, ofp);

    // Clean up everything
    fclose(ifp);
    fclose(ofp);

    //remove(inputfilename);

    free(inputfilename);
    free(outputfilename);

    return lex;
}
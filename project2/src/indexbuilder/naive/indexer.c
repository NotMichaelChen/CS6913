#include "indexer.h"

#include <string.h>
#include <stdio.h>

#include "michaellib/string.h"
#include "michaellib/ulongvec.h"
#include "compression/varbyte.h"
#include "posting.h"

//TODO: bad name
// Generate an index through "ofp" by reading sorted postings from "ifp"
Lexicon* generateIndex(FILE* ifp, FILE* ofp) {
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
        char* linewalker = line;

        // Tokenize the term
        linewalker = strtok(line, "\t\r\n ");
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
                // Add entry to lexicon
                lexicon_insert(&lex, string_getString(currentterm), string_getLen(currentterm), ftell(ofp));

                // Write out posting list
                for(int i = 0; i < ulongvector_size(postinglist); i++) {
                    String* str = varbyte_encode(ulongvector_get(postinglist, i));
                    fwrite(string_getString(str), 1, string_getLen(str), ofp);
                    string_free(str);
                }

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
    for(int i = 0; i < ulongvector_size(postinglist); i++) {
        String* str = varbyte_encode(ulongvector_get(postinglist, i));
        fwrite(string_getString(str), 1, string_getLen(str), ofp);
        string_free(str);
    }

    free(line);
    string_free(currentterm);
    ulongvector_free(postinglist);

    return lex;
}

// Handles file path generation and passing arguments to the generateIndex
// function
Lexicon* naive_buildIndex(char* dir, char* input, char* output) {
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
    Lexicon* lex = generateIndex(ifp, ofp);

    // Clean up everything
    fclose(ifp);
    fclose(ofp);

    remove(inputfilename);

    free(inputfilename);
    free(outputfilename);

    return lex;
}
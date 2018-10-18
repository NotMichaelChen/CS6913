#include "indexer.h"

#include <string.h>
#include <stdio.h>

#include "michaellib/string.h"
#include "michaellib/ulongvec.h"
#include "compression/varbyte.h"
#include "posting.h"

//TODO: bad name
Lexicon* generateIndex(FILE* ifp, FILE* ofp) {
    Lexicon* lex = lexicon_new();

    String* currentterm = string_newstr("");
    ULongVector* postinglist = ulongvector_new();

    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    while ((read = getline(&line, &linelen, ifp)) != -1) {
        char* linewalker = line;

        linewalker = strtok(line, "\t\r\n ");
        if(linewalker == NULL)
            return NULL;
        
        // New term not equal to old term
        if(strcmp(linewalker, string_getString(currentterm))) {
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
        ulongvector_append(postinglist, strtoull(linewalker, NULL, 10));
        linewalker = strtok(NULL, "\t\r\n ");
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

Lexicon* naive_buildIndex(char* dir, char* input, char* output) {
    size_t dirlen = strlen(dir);
    size_t inputlen = strlen(input);
    size_t outputlen = strlen(output);

    char* inputfilename = malloc(dirlen + inputlen + 2);
    snprintf(inputfilename, dirlen+inputlen+2, "%s/%s", dir, input);

    char* outputfilename = malloc(dirlen + outputlen + 2);
    snprintf(outputfilename, dirlen+outputlen+2, "%s/%s", dir, output);

    FILE* ifp = fopen(inputfilename, "r");
    FILE* ofp = fopen(outputfilename, "w");

    Lexicon* lex = generateIndex(ifp, ofp);

    fclose(ifp);
    fclose(ofp);

    remove(inputfilename);

    free(inputfilename);
    free(outputfilename);

    return lex;
}
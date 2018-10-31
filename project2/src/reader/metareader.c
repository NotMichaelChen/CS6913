#include "metareader.h"

#include <stdlib.h>
#include <string.h>

int reader_readMetadata(ReaderMetadata* metadata, FILE* fp) {
    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    // Read line by line
    while((read = getline(&line, &linelen, fp)) != -1) {
        // Attempt to tokenize the field name. Return failure if unable to
        char* linewalker = strtok(line, ":\r\n ");
        if(linewalker == NULL)
            return 1;

        // Save the field name string before continuing to tokenize
        char* key = linewalker;

        // Attempt to tokenize the contents of the fields. Note that we don't
        // split on ":" in this call since any other ":" characters are part of
        // the field value.
        linewalker = strtok(NULL, "\r\n ");

        // Save the field value and get its length, to be used for malloc-ing
        // a new string later
        char* value = linewalker;
        size_t valuelen = strlen(value);

        // Attempt to match the field name on one of the possible field names
        // If a match is found, copy the value string into the relevant metadata
        // struct
        if(!strcmp(key, "WARC-Type")) {
            metadata->WARC_Type = malloc(valuelen + 1);
            strcpy(metadata->WARC_Type, value);
        }
        else if(!strcmp(key, "WARC-Target-URI")) {
            metadata->WARC_Target_URI = malloc(valuelen + 1);
            strcpy(metadata->WARC_Target_URI, value);
        }
        else if(!strcmp(key, "WARC-Date")) {
            metadata->WARC_Date = malloc(valuelen + 1);
            strcpy(metadata->WARC_Date, value);
        }
        else if(!strcmp(key, "WARC-Record-ID")) {
            metadata->WARC_Record_ID = malloc(valuelen + 1);
            strcpy(metadata->WARC_Record_ID, value);
        }
        else if(!strcmp(key, "WARC-Refers-To")) {
            metadata->WARC_Refers_To = malloc(valuelen + 1);
            strcpy(metadata->WARC_Refers_To, value);
        }
        else if(!strcmp(key, "WARC-Block-Digest")) {
            metadata->WARC_Block_Digest = malloc(valuelen + 1);
            strcpy(metadata->WARC_Block_Digest, value);
        }
        else if(!strcmp(key, "Content-Type")) {
            metadata->Content_Type = malloc(valuelen + 1);
            strcpy(metadata->Content_Type, value);
        }
        else if(!strcmp(key, "Content-Length")) {
            metadata->Content_Length = malloc(valuelen + 1);
            strcpy(metadata->Content_Length, value);
            // Assumed to be the last entry in the metadata block, so stop
            // parsing here
            break;
        }
        else {
            free(line);
            return 2;
        }
    }

    //Read one more line to get to the actual document content
    read = getline(&line, &linelen, fp);
    free(line);
    if(read < 0)
        return 3;
    
    return 0;
}

void reader_freeMetadata(ReaderMetadata* metadata) {
    free(metadata->WARC_Type);
    free(metadata->WARC_Target_URI);
    free(metadata->WARC_Date);
    free(metadata->WARC_Record_ID);
    free(metadata->WARC_Refers_To);
    free(metadata->WARC_Block_Digest);
    free(metadata->Content_Type);
    free(metadata->Content_Length);
}
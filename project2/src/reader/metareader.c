#include "metareader.h"

#include <stdlib.h>
#include <string.h>

int reader_readMetadata(ReaderMetadata* metadata, FILE* fp) {
    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    while((read = getline(&line, &linelen, fp)) != -1) {
        char* linewalker = line;

        linewalker = strtok(line, ":\r\n ");
        if(linewalker == NULL)
            return 1;

        char* key = linewalker;

        linewalker = strtok(NULL, "\r\n ");

        char* value = linewalker;
        size_t valuelen = strlen(value);

        if(!strcmp(key, "WARC-Type")) {
            metadata->WARC_Type = malloc(valuelen + 1);
            strncpy(metadata->WARC_Type, value, valuelen+1);
        }
        else if(!strcmp(key, "WARC-Target-URI")) {
            metadata->WARC_Target_URI = malloc(valuelen + 1);
            strncpy(metadata->WARC_Target_URI, value, valuelen+1);
        }
        else if(!strcmp(key, "WARC-Date")) {
            metadata->WARC_Date = malloc(valuelen + 1);
            strncpy(metadata->WARC_Date, value, valuelen+1);
        }
        else if(!strcmp(key, "WARC-Record-ID")) {
            metadata->WARC_Record_ID = malloc(valuelen + 1);
            strncpy(metadata->WARC_Record_ID, value, valuelen+1);
        }
        else if(!strcmp(key, "WARC-Refers-To")) {
            metadata->WARC_Refers_To = malloc(valuelen + 1);
            strncpy(metadata->WARC_Refers_To, value, valuelen+1);
        }
        else if(!strcmp(key, "WARC-Block-Digest")) {
            metadata->WARC_Block_Digest = malloc(valuelen + 1);
            strncpy(metadata->WARC_Block_Digest, value, valuelen+1);
        }
        else if(!strcmp(key, "Content-Type")) {
            metadata->Content_Type = malloc(valuelen + 1);
            strncpy(metadata->Content_Type, value, valuelen+1);
        }
        else if(!strcmp(key, "Content-Length")) {
            metadata->Content_Length = malloc(valuelen + 1);
            strncpy(metadata->Content_Length, value, valuelen+1);
            //Assumed to be the last entry in the metadata block
            break;
        }
        else {
            return 2;
        }
    }

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
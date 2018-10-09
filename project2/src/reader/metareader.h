#pragma once

#include <stdio.h>

typedef struct ReaderMetadata {
    char* WARC_Type;
    char* WARC_Target_URI;
    char* WARC_Date;
    char* WARC_Record_ID;
    char* WARC_Refers_To;
    char* WARC_Block_Digest;
    char* Content_Type;
    char* Content_Length;
} ReaderMetadata;

int reader_readMetadata(ReaderMetadata* metadata, FILE* fp);
// Only frees the contents, if the metadata struct was malloced, the caller must
// free the struct
void reader_freeMetadata(ReaderMetadata* metadata);
#pragma once

#include <stdio.h>

// Represents a collection of all of the metadata fields in the WET file. No
// state is required so the struct can be fully defined in the header

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

// Read the metatadata from the file. Assumes that the WARC/1.0 line has already
// been read and we are currently pointing at one of the metadata fields
int reader_readMetadata(ReaderMetadata* metadata, FILE* fp);
// Only frees the contents. If the metadata struct was malloced, the caller must
// free the struct themselves
void reader_freeMetadata(ReaderMetadata* metadata);
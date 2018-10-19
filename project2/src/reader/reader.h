#pragma once

#include <stdio.h>

// Represents an object that reads from a single WET file and parses out
// documents.

// Represents a document with a url, size, and document contents. Must be freed
// before being updated with getDocument
// TODO: Maybe separate this definition out into its own file
typedef struct Document {
    char* url;
    char* doc;
    int docsize;
} Document;

typedef struct Reader Reader;

Reader* reader_new(FILE* fp);

// Gets the next document in the file
Document reader_getDocument(Reader* reader);
// Returns the status of the reader
int reader_getStatus(Reader* reader);

void reader_free(Reader* reader);
//Only frees contents
void reader_freedoc(Document* doc);
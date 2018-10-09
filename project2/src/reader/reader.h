#pragma once

#include <stdio.h>

typedef struct Document {
    char* url;
    char* doc;
} Document;

typedef struct Reader Reader;

Reader* reader_new(FILE* fp);

Document reader_getDocument(Reader* reader);
int reader_getStatus(Reader* reader);

void reader_free(Reader* reader);
//Only frees contents
void reader_freedoc(Document* doc);
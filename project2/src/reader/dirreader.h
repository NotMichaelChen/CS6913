#pragma once

#include "reader.h"

typedef struct DirReader DirReader;

DirReader* dirreader_new(char* directory);
Document dirreader_getDocument(DirReader* reader);
int dirreader_getStatus(DirReader* reader);

void dirreader_free(DirReader* reader);
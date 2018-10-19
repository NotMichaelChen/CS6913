#pragma once

#include "reader.h"

// Represents an object that reads documents from a directory of WET files.
// Wraps the Reader object that reads documents from a WET file, and manages
// the reader by pointing it to the next file in the directory to continue
// reading documents

typedef struct DirReader DirReader;

DirReader* dirreader_new(char* directory);

// Gets the next document. Automatically traverses files in the folder
Document dirreader_getDocument(DirReader* reader);
// Returns whether the dir reader has any errors or nor
int dirreader_getStatus(DirReader* reader);

void dirreader_free(DirReader* reader);
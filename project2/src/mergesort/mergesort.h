#pragma once

#include <stdlib.h>

// Merges a directory of files into one file called "outputname"
// Note: There should be *NO* files other than the intermediate file in the dir
// Otherwise they'll be deleted after the merge is done
int merge(char* directory, char* outputname, size_t buffer);
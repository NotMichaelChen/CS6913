#pragma once

#include <stdlib.h>

// Note: There should be *NO* files other than the intermediate file in the dir
// Otherwise they'll be deleted after the merge is done
int merge(char* directory, char* outputname, size_t buffer);
#pragma once

#include <stdlib.h>

#include "michaellib/minheap.h"
#include "michaellib/vector/stringvec.h"

int util_getDigitCount(size_t num);
void util_printSnippet(char* doc, StringVec* terms);

// Comparison functions

int util_gcmpHeapEntry(const void* a, const void* b);
#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "michaellib/minheap.h"
#include "michaellib/vector/stringvec.h"

int util_getDigitCount(size_t num);
// Returns whether the term is valid or not
bool util_filterTerm(char* str);
void util_printSnippet(char* doc, StringVec* terms);

// Comparison functions

int util_gcmpHeapEntry(const void* a, const void* b);
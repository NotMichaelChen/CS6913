#pragma once

#include <stdlib.h>

#include "michaellib/minheap.h"

int util_getDigitCount(size_t num);

// Comparison functions

int util_gcmpHeapEntry(const void* a, const void* b);
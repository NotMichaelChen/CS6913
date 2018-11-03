#pragma once

#include <stdlib.h>

typedef struct MinHeap MinHeap;

MinHeap* minheap_new(size_t count);

void minheap_attemptInsert(MinHeap* heap, double num);
size_t minheap_len(MinHeap* heap);
double* minheap_getArr(MinHeap* heap);

void minheap_free(MinHeap* heap);
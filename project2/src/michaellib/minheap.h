#pragma once

#include <stdlib.h>

#include "global.h"

typedef struct {
    double score;
    docID_t docID;
} HeapEntry;

typedef struct MinHeap MinHeap;

MinHeap* minheap_new(size_t count);

void minheap_attemptInsert(MinHeap* heap, HeapEntry entry);
size_t minheap_len(MinHeap* heap);
HeapEntry* minheap_getArr(MinHeap* heap);

void minheap_free(MinHeap* heap);
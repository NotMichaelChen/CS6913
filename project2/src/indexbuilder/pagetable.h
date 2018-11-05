#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "michaellib/string.h"
#include "global.h"

// Represents the page table structure

typedef struct PageTable PageTable;

PageTable* pagetable_new();

// Assumes pages are inserted in increasing doc order
void pagetable_add(PageTable* table, char* url, size_t termcount, char* wetpath, size_t doclen, size_t offset);

String* pagetable_geturl(PageTable* table, docID_t docID);
uint32_t pagetable_getTermCount(PageTable* table, size_t docID);
double pagetable_getAvgTermCount(PageTable* table);
// WARNING - returns allocated memory
char* pagetable_getDocument(PageTable* table, size_t docID);
size_t pagetable_len(PageTable* table);

void pagetable_read(PageTable* table, FILE* fp);
void pagetable_dump(PageTable* table, FILE* fp);

void pagetable_free(PageTable* table);
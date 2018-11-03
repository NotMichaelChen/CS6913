#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// Represents the page table structure

typedef struct PageTable PageTable;

PageTable* pagetable_new();

// Assumes pages are inserted in increasing doc order
void pagetable_add(PageTable* table, char* url, size_t pagesize);

uint32_t pagetable_getPageLength(PageTable* table, size_t docID);
double pagetable_getAvgPageLength(PageTable* table);
size_t pagetable_len(PageTable* table);

void pagetable_dump(PageTable* table, FILE* fp);

void pagetable_free(PageTable* table);
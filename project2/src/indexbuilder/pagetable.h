#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct PageTable PageTable;

PageTable* pagetable_new();

// Assumes pages are inserted in increasing doc order
void pagetable_add(PageTable* table, char* url, size_t pagesize);
void pagetable_dump(PageTable* table, FILE* fp);

void pagetable_free(PageTable* table);
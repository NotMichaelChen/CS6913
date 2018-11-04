#define _XOPEN_SOURCE 700
#include "pagetable.h"

#include <string.h>

typedef struct TableEntry {
    String* url;
    uint32_t pagelength;
} TableEntry;

struct PageTable {
    TableEntry* list;
    double avgpagelength;
    size_t size;
    size_t cap;
};

PageTable* pagetable_new() {
    PageTable* table = malloc(sizeof(PageTable));

    // Since I know I'm going to insert stuff, I can initialize with a value
    table->list = malloc(sizeof(TableEntry) * 2);
    table->size = 0;
    table->cap = 2;
    table->avgpagelength = 0;

    return table;
}

void pagetable_add(PageTable* table, char* url, size_t pagelength) {
    if(table->size == table->cap) {
        table->cap *= 2;
        table->list = realloc(table->list, sizeof(TableEntry) * table->cap);
    }

    table->list[table->size].url = string_newstr(url);
    table->list[table->size].pagelength = pagelength;
    table->avgpagelength += (pagelength - table->avgpagelength) / (table->size+1);

    table->size++;
}

String* pagetable_geturl(PageTable* table, docID_t docID) {
    return table->list[docID].url;
}

uint32_t pagetable_getPageLength(PageTable* table, size_t docID) {
    return table->list[docID].pagelength;
}

double pagetable_getAvgPageLength(PageTable* table) {
    return table->avgpagelength;
}

size_t pagetable_len(PageTable* table) {
    return table->size;
}

void pagetable_read(PageTable* table, FILE* fp) {
    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    while ((read = getline(&line, &linelen, fp)) != -1) {
        char* linewalker = strtok(line, "\t\r\n ");
        if(linewalker == NULL)
            return;
        
        char* url = linewalker;

        linewalker = strtok(NULL, "\t\r\n ");
        uint32_t pagelen = strtoul(linewalker, NULL, 10);

        pagetable_add(table, url, pagelen);
    }

    free(line);
}

void pagetable_dump(PageTable* table, FILE* fp) {
    for(size_t i = 0; i < table->size; i++) {
        fprintf(fp, "%s %u\n", string_getString(table->list[i].url), table->list[i].pagelength);
    }
}

void pagetable_free(PageTable* table) {
    for(size_t i = 0; i < table->size; i++) {
        string_free(table->list[i].url);
    }

    free(table->list);
    free(table);
}
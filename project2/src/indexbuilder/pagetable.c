#define _XOPEN_SOURCE 700
#include "pagetable.h"

#include <string.h>

typedef struct TableEntry {
    String* url;
    uint32_t termcount;
    String* wetpath;
    size_t doclen;
    size_t offset;
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

void pagetable_add(PageTable* table, char* url, size_t termcount, char* wetpath, size_t doclen, size_t offset) {
    if(table->size == table->cap) {
        table->cap *= 2;
        table->list = realloc(table->list, sizeof(TableEntry) * table->cap);
    }

    table->list[table->size].url = string_newstr(url);
    table->list[table->size].termcount = termcount;
    table->list[table->size].wetpath = string_newstr(wetpath);
    table->list[table->size].doclen = doclen;
    table->list[table->size].offset = offset;
    table->avgpagelength += (termcount - table->avgpagelength) / (table->size+1);

    table->size++;
}

String* pagetable_geturl(PageTable* table, docID_t docID) {
    return table->list[docID].url;
}

uint32_t pagetable_getTermCount(PageTable* table, size_t docID) {
    return table->list[docID].termcount;
}

double pagetable_getAvgTermCount(PageTable* table) {
    return table->avgpagelength;
}

size_t pagetable_len(PageTable* table) {
    return table->size;
}

char* pagetable_getDocument(PageTable* table, size_t docID) {
    FILE* fp = fopen(string_getString(table->list[docID].wetpath), "rb");
    fseek(fp, table->list[docID].offset, SEEK_SET);

    char* doc = malloc(table->list[docID].doclen);
    fread(doc, 1, table->list[docID].doclen, fp);

    fclose(fp);

    return doc;
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

        linewalker = strtok(NULL, "\t\r\n ");
        char* wetpath = linewalker;

        linewalker = strtok(NULL, "\t\r\n ");
        size_t doclen = strtoull(linewalker, NULL, 10);

        linewalker = strtok(NULL, "\t\r\n ");
        size_t offset = strtoull(linewalker, NULL, 10);

        pagetable_add(table, url, pagelen, wetpath, doclen, offset);
    }

    free(line);
}

void pagetable_dump(PageTable* table, FILE* fp) {
    for(size_t i = 0; i < table->size; i++) {
        fprintf(fp, "%s %u %s %lu %lu\n",
            string_getString(table->list[i].url),
            table->list[i].termcount,
            string_getString(table->list[i].wetpath),
            table->list[i].doclen,
            table->list[i].offset
        );
    }
}

void pagetable_free(PageTable* table) {
    for(size_t i = 0; i < table->size; i++) {
        string_free(table->list[i].url);
        string_free(table->list[i].wetpath);
    }

    free(table->list);
    free(table);
}
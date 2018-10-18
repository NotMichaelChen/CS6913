#include "pagetable.h"

#include "michaellib/string.h"

typedef struct TableEntry {
    String* url;
    size_t pagesize;
} TableEntry;

struct PageTable {
    TableEntry* list;
    size_t size;
    size_t cap;
};

PageTable* pagetable_new() {
    PageTable* table = malloc(sizeof(PageTable));

    // Since I know I'm going to insert stuff, I can initialize with a value
    table->list = malloc(sizeof(TableEntry) * 2);
    table->size = 0;
    table->cap = 2;

    return table;
}

void pagetable_add(PageTable* table, char* url, size_t pagesize) {
    if(table->size == table->cap) {
        table->cap *= 2;
        table->list = realloc(table->list, sizeof(TableEntry) * table->cap);
    }

    table->list[table->size].url = string_newstr(url);
    table->list[table->size].pagesize = pagesize;

    table->size++;
}

void pagetable_dump(PageTable* table, FILE* fp) {
    for(size_t i = 0; i < table->size; i++) {
        fprintf(fp, "%lu %s %lu\n", i, string_getString(table->list[i].url), table->list[i].pagesize);
    }
}

void pagetable_free(PageTable* table) {
    for(size_t i = 0; i < table->size; i++) {
        string_free(table->list[i].url);
    }

    free(table);
}
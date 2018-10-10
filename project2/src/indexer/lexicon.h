#pragma once

#include <stdint.h>

#include "lib/uthash.h"
#include "michaellib/string.h"

typedef struct Lexicon Lexicon;

typedef struct LexiconEntry {
    char* term;
    uint32_t pos;
    UT_hash_handle hh;
} LexiconEntry;

Lexicon* lexicon_new();

void lexicon_update(Lexicon* lex, String* term, int layer, int filenum, uint32_t newpos);

void lexicon_free(Lexicon* lex);
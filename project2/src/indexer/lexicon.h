#pragma once

#include <stdint.h>

#include "lib/uthash.h"
#include "michaellib/string.h"

typedef struct Lexicon Lexicon;

typedef struct LexiconEntry {
    char* term;
    size_t pos;
    UT_hash_handle hh;
} LexiconEntry;

Lexicon* lexicon_new();

LexiconEntry* lexicon_get(Lexicon* lex, int layer, int filenum);
void lexicon_clear(Lexicon* lex, int layer);

void lexicon_free(Lexicon* lex);
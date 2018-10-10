#pragma once

#include <stdint.h>

#include "michaellib/string.h"

typedef struct Lexicon Lexicon;

Lexicon* lexicon_new();

void lexicon_update(Lexicon* lex, String* term, int layer, int filenum, uint32_t newpos);

void lexicon_free(Lexicon* lex);
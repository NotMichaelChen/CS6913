#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct Lexicon Lexicon;

Lexicon* lexicon_new();

void lexicon_insert(Lexicon* lex, char* term, size_t termlen, size_t pos);
size_t lexicon_get(Lexicon* lex, char* term);
void lexicon_dump(Lexicon* lex, FILE* fp);

void lexicon_free(Lexicon* lex);
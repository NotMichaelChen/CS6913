#pragma once

#include <stdlib.h>
#include <stdio.h>

// Wraps the lexicon dictionary in a more intuitive interface. Note that all of
// the function take a pointer to a lexicon pointer, since uthash needs to
// be able to change the head pointer of the dictionary

typedef struct Lexicon Lexicon;

Lexicon* lexicon_new();

void lexicon_insert(Lexicon** lex, char* term, size_t termlen, size_t pos);
size_t lexicon_get(Lexicon** lex, char* term);
void lexicon_dump(Lexicon** lex, FILE* fp);

void lexicon_free(Lexicon** lex);
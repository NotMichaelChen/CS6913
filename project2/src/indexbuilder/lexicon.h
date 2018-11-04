#pragma once

#include <stdlib.h>
#include <stdio.h>

// Wraps the lexicon dictionary in a more intuitive interface. Note that all of
// the function take a pointer to a lexicon pointer, since uthash needs to
// be able to change the head pointer of the dictionary

typedef struct Lexicon Lexicon;

Lexicon* lexicon_new();

void lexicon_insert(Lexicon* lex, char* term, size_t termlen, size_t pos, size_t metasize, size_t docscontaining);
//TODO: determine if combining the getter functions would be better
size_t lexicon_getpos(Lexicon* lex, char* term);
size_t lexicon_getmetasize(Lexicon* lex, char* term);
size_t lexicon_getlistlen(Lexicon* lex, char* term);

void lexicon_read(Lexicon* lex, FILE* fp);
void lexicon_dump(Lexicon* lex, FILE* fp);

void lexicon_free(Lexicon* lex);
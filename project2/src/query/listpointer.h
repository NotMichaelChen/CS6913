#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "indexbuilder/lexicon.h"
#include "global.h"

typedef struct ListPointer ListPointer;

ListPointer* listpointer_open(char* term, Lexicon* lex, FILE* fp);

docID_t listpointer_nextGEQ(ListPointer* lp, docID_t docID, bool* success);
freq_t listpointer_getFreq(ListPointer* lp);

void listpointer_free(ListPointer* lp);
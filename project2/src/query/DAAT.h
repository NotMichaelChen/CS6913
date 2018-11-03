#pragma once

#include <stdlib.h>

#include "indexbuilder/pagetable.h"
#include "indexbuilder/lexicon.h"

void DAAT(char** terms, size_t termcount, Lexicon* lex, PageTable* pagetable, FILE* fp);
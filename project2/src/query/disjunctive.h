#pragma once

#include <stdlib.h>

#include "indexbuilder/pagetable.h"
#include "indexbuilder/lexicon.h"
#include "michaellib/minheap.h"

MinHeap* disjunctive_query(char** terms, size_t termcount, Lexicon* lex, PageTable* pagetable, FILE* fp);
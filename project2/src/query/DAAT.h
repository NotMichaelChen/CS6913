#pragma once

#include <stdlib.h>

#include "indexbuilder/pagetable.h"
#include "indexbuilder/lexicon.h"
#include "michaellib/minheap.h"

MinHeap* DAAT(char** terms, size_t termcount, Lexicon* lex, PageTable* pagetable, String* indexpath);
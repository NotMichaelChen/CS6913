#pragma once

#include <stdint.h>

#include "reader/reader.h"
#include "lexicon.h"

void docwriter_writeDoc(Document doc, size_t docID, FILE* fp, LexiconEntry* lexent);
#pragma once

#include "indexbuilder/lexicon.h"

// Implements the block-based index building algorithm, which outputs postings
// lists as <docID, freq> blocks

Lexicon* block_buildIndex(char* dir, char* input, char* output);
#pragma once

#include "indexbuilder/lexicon.h"

// Implements the naive index building algorithm, which simply outputs posting
// lists as <docID, freq> pairs

Lexicon* naive_buildIndex(char* dir, char* input, char* output);
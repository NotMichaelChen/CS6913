#pragma once

#include <stdint.h>
#include <stdio.h>

#include "docparser.h"
#include "lexicon.h"
#include "michaellib/string.h"

//fp MUST be pointing to the beginning of an empty file
void writePostings(IntermediatePostingList postinglist, uint32_t docID, FILE* fp, LexiconEntry* lexent);
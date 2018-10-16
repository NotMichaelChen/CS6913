#pragma once

#include "reader/reader.h"

typedef struct IndexBuilder IndexBuilder;

//output directory
IndexBuilder* indexbuilder_new(char* directory);

void indexbuilder_addDoc(IndexBuilder* builder, Document doc);
void indexbuilder_mergeAll(IndexBuilder* builder);

void indexbuilder_free(IndexBuilder* builder)
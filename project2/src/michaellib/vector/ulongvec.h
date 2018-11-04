#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct ULongVector ULongVector;

ULongVector* ulongvector_new();

void ulongvector_append(ULongVector* vec, uint64_t num);
uint64_t ulongvector_get(ULongVector* vec, size_t pos);
size_t ulongvector_size(ULongVector* vec);
uint64_t* ulongvector_getBuffer(ULongVector* vec);
void ulongvector_clear(ULongVector* vec);

void ulongvector_free(ULongVector* vec);
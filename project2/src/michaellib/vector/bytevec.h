#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "michaellib/string.h"

typedef struct ByteVec ByteVec;

ByteVec* bytevec_new();

void bytevec_append(ByteVec* vec, uint8_t byte);
void bytevec_appendstr(ByteVec* vec, String* str);
uint8_t bytevec_get(ByteVec* vec, size_t pos);
size_t bytevec_len(ByteVec* vec);
void bytevec_clear(ByteVec* vec);
void bytevec_dump(ByteVec* vec, FILE* fp);

void bytevec_free(ByteVec* vec);
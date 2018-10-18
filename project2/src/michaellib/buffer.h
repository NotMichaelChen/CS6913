#pragma once

#include <stdint.h>
#include <stdio.h>

#include "posting.h"

typedef struct Buffer Buffer;

Buffer* buffer_new(size_t size);

char* buffer_getStart(Buffer* buf);

//Will silently refuse to write if data cannot fit into buffer
//Data should follow term-docid-freq format
void buffer_write(Buffer* buf, char* data, size_t len);
size_t buffer_getRemaining(Buffer* buf);
// Places number of elements in count
// Be sure to free the memposting array when you're done
MemPosting* buffer_getPostings(Buffer* buf, size_t* count);
void buffer_clear(Buffer* buf);

void buffer_free(Buffer* buf);
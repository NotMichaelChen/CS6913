#pragma once

#include <stdint.h>
#include <stdio.h>

#include "posting.h"

// Represents the buffer of postings used during intermediate file generation
// Stored as a simple array of bytes, but allows that array of bytes to be
// interpreted as a list of postings

typedef struct Buffer Buffer;

Buffer* buffer_new(size_t size);

// Gets a pointer to the beginning of the buffer
char* buffer_getStart(Buffer* buf);

// Writes some data with some length to the buffer
// Will silently refuse to write if data cannot fit into buffer
// Data should follow term-docid-freq format
void buffer_write(Buffer* buf, char* data, size_t len);
// Get the remaining number of bytes in the array
size_t buffer_getRemaining(Buffer* buf);
// Interprets the buffer as an array of postings
// Places number of elements in count
// Be sure to free the memposting array when you're done
MemPosting* buffer_getPostings(Buffer* buf, size_t* count);
// "Clears" the buffer by setting the read head back to the beginning
void buffer_clear(Buffer* buf);

void buffer_free(Buffer* buf);
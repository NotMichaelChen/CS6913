#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct Buffer Buffer;

Buffer* buffer_new(uint32_t size);

char* buffer_getStart(Buffer* buf);

//Will silently refuse to write if data cannot fit into buffer
void buffer_write(Buffer* buf, char* data, uint32_t len);
int buffer_getRemaining(Buffer* buf);
void buffer_flush(Buffer* buf, FILE* fp);
void buffer_clear(Buffer* buf);

void buffer_free(Buffer* buf);
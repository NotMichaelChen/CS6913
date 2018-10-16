#include "buffer.h"

#include <stdlib.h>

struct Buffer {
    char* buffer;
    
    //Points to the next available spot to write to
    char* head;
    uint32_t pos;

    uint32_t len;
};

Buffer* buffer_new(uint32_t size) {
    Buffer* buf = malloc(sizeof(Buffer));

    buf->buffer = malloc(size);
    if(buf->buffer == NULL) {
        printf("Error, unable to allocate buffer of size %u\n", size);
        exit(1);
    }

    buf->head = buf->buffer;
    buf->pos = 0;

    buf->len = size;
}

char* buffer_getStart(Buffer* buf) {
    return buf->buffer;
}

void buffer_write(Buffer* buf, char* data, uint32_t len) {
    if(buf->pos + len > buf->len)
        return;
    
    memcpy(buf->head, data, len);

    buf->head += len;
    buf->pos += len;
}

int buffer_getRemaining(Buffer* buf) {
    return buf->len - buf->pos;
}

void buffer_flush(Buffer* buf, FILE* fp) {
    fwrite(buf->buffer, sizeof(char), buf->pos, fp);
    buffer_clear(buf);
}

void buffer_clear(Buffer* buf) {
    buf->head = buf->buffer;
    buf->pos = 0;
}

void buffer_free(Buffer* buf) {
    free(buf->buffer);
    free(buf);
}
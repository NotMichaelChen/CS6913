#include "buffer.h"

#include <stdlib.h>
#include <string.h>

#include "michaellib/string.h"

struct Buffer {
    char* buffer;
    
    //Points to the next available spot to write to
    char* head;
    size_t pos;

    size_t len;
    size_t writecount;
};

Buffer* buffer_new(size_t size) {
    Buffer* buf = malloc(sizeof(Buffer));

    buf->len = size;
    buf->buffer = malloc(buf->len);
    if(buf->buffer == NULL) {
        printf("Error, unable to allocate buffer of size %lu\n", size);
        exit(1);
    }

    buf->head = buf->buffer;
    buf->pos = 0;
    buf->writecount = 0;


    return buf;
}

char* buffer_getStart(Buffer* buf) {
    return buf->buffer;
}

void buffer_write(Buffer* buf, char* data, size_t len) {
    if(buf->pos + len > buf->len)
        return;
    
    memcpy(buf->head, data, len);

    buf->head += len;
    buf->pos += len;
    buf->writecount += 1;
}

size_t buffer_getRemaining(Buffer* buf) {
    return buf->len - buf->pos;
}

MemPosting* buffer_getPostings(Buffer* buf, size_t* count) {
    MemPosting* postinglist = malloc(sizeof(MemPosting) * buf->writecount);

    size_t index = 0;
    char* start = buf->buffer;
    char* walker = start;

    while(walker != buf->head && index < buf->writecount) {
        if(*walker == 0) {
            postinglist[index].term = string_newstr(start);
            walker++;
            memcpy(&(postinglist[index].docID), walker, sizeof(postinglist[index].docID));
            walker += sizeof(postinglist[index].docID);
            memcpy(&(postinglist[index].freq), walker, sizeof(postinglist[index].freq));
            walker += sizeof(postinglist[index].freq);

            start = walker;

            index++;
        }
        else {
            walker++;
        }
    }

    *count = index;
    
    buffer_clear(buf);

    return postinglist;
}

void buffer_clear(Buffer* buf) {
    buf->head = buf->buffer;
    buf->pos = 0;
    buf->writecount = 0;
}

void buffer_free(Buffer* buf) {
    free(buf->buffer);
    free(buf);
}
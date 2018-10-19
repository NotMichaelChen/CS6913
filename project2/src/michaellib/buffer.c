#include "buffer.h"

#include <stdlib.h>
#include <string.h>

#include "michaellib/string.h"

struct Buffer {
    char* buffer;
    
    // Points to the next available spot to write to
    char* head;
    // Represents how many bytes have already been written
    size_t pos;

    // Represents how many bytes are available in the buffer
    size_t len;
    // How many times the buffer has been written to
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
    // Fail silently if not enough space in buffer
    if(buf->pos + len > buf->len)
        return;
    
    // Copy data into buffer
    memcpy(buf->head, data, len);

    // Update stats
    buf->head += len;
    buf->pos += len;
    buf->writecount += 1;
}

size_t buffer_getRemaining(Buffer* buf) {
    // Total size - Used size
    return buf->len - buf->pos;
}

MemPosting* buffer_getPostings(Buffer* buf, size_t* count) {
    // Allocate the posting list
    MemPosting* postinglist = malloc(sizeof(MemPosting) * buf->writecount);

    size_t index = 0;
    // Represents the start of the posting segment
    char* start = buf->buffer;
    // Walks along the buffer until it knows it's at the end of the posting
    // segment
    char* walker = start;

    // Loop through the buffer until we go past the end of the written part of
    // the buffer, or we write enough postings to the posting list
    while(walker != buf->head && index < buf->writecount) {
        // If we encounter a null terminator, we know we're at the end of the
        // string
        if(*walker == 0) {
            // Copy the string, starting from the beginning of the posting
            // segment
            postinglist[index].term = string_newstr(start);
            // Go past the null terminator
            walker++;
            // Copy the docID into the posting
            memcpy(&(postinglist[index].docID), walker, sizeof(postinglist[index].docID));
            // Walk past the docID
            walker += sizeof(postinglist[index].docID);
            // Copy the frequency into the posting
            memcpy(&(postinglist[index].freq), walker, sizeof(postinglist[index].freq));
            // Walk past the frequency
            walker += sizeof(postinglist[index].freq);

            // Mark the start of a new posting segment
            start = walker;

            // Increment the next location to write to
            index++;
        }
        // Otherwise keep walking
        else {
            walker++;
        }
    }

    // Set the count variable to let the caller know how long the array is
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
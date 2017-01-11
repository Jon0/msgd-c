#include <stdio.h>
#include <string.h>
#include <math.h>

#include "buffer.h"


void msgu_buffer_init_default(struct msgu_buffer *b) {
    msgu_buffer_init(b, malloc(EP_BUFFER_DEFAULT_SIZE), EP_BUFFER_DEFAULT_SIZE);
}


void msgu_buffer_init(struct msgu_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->size = 0;
    b->esize = 1;
}


ssize_t msgu_buffer_read(struct msgu_buffer *b, char *outbuf, size_t count) {
    ssize_t s = ep_buffer_peek(b, outbuf, 0, count);
    ep_buffer_release(b, s);
    return s;
}


ssize_t msgu_buffer_write(struct msgu_buffer *b, const char *inbuf, size_t count) {
    size_t end = b->begin + b->size;

    // check if insert will exceed capacity
    if (count > (b->avail - b->size)) {
        count = b->avail - b->size;
    }

    if (end < b->avail) {
        char *back = &b->ptr[end];
        size_t cs = b->avail - end;
        if (count > cs) {
            memcpy(back, inbuf, cs);
            memcpy(b->ptr, &inbuf[cs], count - cs);
        }
        else {
            memcpy(back, inbuf, count);
        }
    }
    else {
        char *back = &b->ptr[end - b->avail];
        memcpy(back, inbuf, count);
    }
    b->size += count;
    return count;
}

void ep_buffer_wrap(struct msgu_buffer *b, char *buf, size_t count) {
    b->ptr = buf;
    b->avail = count;
    b->begin = 0;
    b->size = count;
    b->esize = 1;
}


void ep_buffer_resize(struct msgu_buffer *b, size_t newsize) {
    char *newmem = malloc(newsize);
    if (newsize < b->size) {
        b->size = ep_buffer_peek(b, newmem, 0, newsize);
    }
    else {
        b->size = ep_buffer_peek(b, newmem, 0, b->size);
    }
    b->ptr = newmem;
    b->avail = newsize;
    b->begin = 0;
}


void ep_buffer_free(struct msgu_buffer *b) {
    b->avail = 0;
    b->begin = 0;
    b->size = 0;
    free(b->ptr);
}


void ep_buffer_endmem(struct msgu_buffer *b, char **end, size_t *space) {
    size_t end_index = b->begin + b->size;
    if (end_index < b->avail) {
        *end = &b->ptr[end_index];
        *space = b->avail - end_index;
    }
    else {
        *end = &b->ptr[end_index - b->avail];
        *space = b->avail - b->size;
    }
}


void ep_buffer_clear(struct msgu_buffer *b) {
    b->begin = 0;
    b->size = 0;
}


size_t ep_buffer_copy(struct msgu_buffer *outbuf, struct msgu_buffer *inbuf, size_t start) {
    printf("TODO: copy\n");
    return 0;
}


size_t ep_buffer_peek(struct msgu_buffer *b, char *outbuf, size_t offset, size_t count) {
    size_t begin = b->begin + offset;
    size_t end = b->begin + b->size;
    size_t total = offset + count;
    size_t cs = b->avail - begin; // length of first continuous block
    char *data = &b->ptr[begin];

    // check size of copy matches size of buffer
    if (total > b->size) {
        total = b->size;
        if (b->size > offset) {
            count = b->size - offset;
        }
        else {
            return 0;
        }
    }

    // copy in one or two operations
    // depending how data is aligned in buffer
    if (count > cs) {
        memcpy(outbuf, data, cs);
        memcpy(&outbuf[cs], b->ptr, count - cs);
    }
    else {
        memcpy(outbuf, data, count);
    }
    return count;
}


void ep_buffer_release(struct msgu_buffer *b, size_t count) {
    if (count < b->size) {
        b->begin = (b->begin + count) % b->avail;
        b->size -= count;
    }
    else {
        b->begin = 0;
        b->size = 0;
    }
}

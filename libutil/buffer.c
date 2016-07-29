#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "buffer.h"


void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->size = 0;
}


void ep_buffer_endmem(struct ep_buffer *b, char **end, size_t *space) {
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


size_t ep_buffer_insert(struct ep_buffer *b, const char *inbuf, size_t count) {
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


size_t ep_buffer_erase(struct ep_buffer *b, char *outbuf, size_t count) {
    size_t end = b->begin + b->size;
    if (count > b->size) {
        count = b->size;
    }

    char *start = &b->ptr[b->begin];
    size_t cs = b->avail - b->begin;
    if (count > cs) {
        memcpy(outbuf, start, cs);
        memcpy(&outbuf[cs], b->ptr, count - cs);
    }
    else {
        memcpy(outbuf, start, count);
    }
    ep_buffer_release(b, count);
    return count;
}


ssize_t ep_buffer_take(struct ep_buffer *b, int fd) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    // try read from file descriptor
    ssize_t r = read(fd, back, space);
    if (r < 0) {
        perror("read");
    }
    else {
        b->size += r;
    }
    printf("buffer read %d, (%d / %d used)\n", r, b->size, b->avail);
    return r;
}


size_t ep_buffer_take_src(struct ep_buffer *b, int fd, size_t count) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    if (count > space) {
        count = space;
    }

    ssize_t r = read(fd, back, count);
    if (r < 0) {
        perror("read");
        return 0;
    }
    else {
        b->size += r;
        return r;
    }
}


ssize_t ep_buffer_write(struct ep_buffer *b, int fd, size_t begin) {

    // write as much as possible from begin
    size_t end = b->begin + b->size;
    if (end < b->avail) {
        return write(fd, &b->ptr[begin], end - begin);
    }
    else if (begin < (end - b->avail)) {
        // begin is positioned before end, only one write is required
        return write(fd, &b->ptr[begin], (end - b->avail) - begin);
    }
    else {
        // begin is positioned after end, not all can be written
        return write(fd, &b->ptr[begin], (b->begin - b->avail));
    }
}


size_t ep_buffer_write_inc(struct ep_buffer *b, int fd, size_t *begin) {
    ssize_t w = ep_buffer_write(b, fd, *begin);
    if (w > 0) {
        *begin = (*begin + w) % b->avail;
    }

    // return remainder
    return ((b->avail + b->begin + b->size) - *begin) % b->avail;
}


void ep_buffer_release(struct ep_buffer *b, size_t count) {
    if (count < b->size) {
        b->begin = (b->begin + count) % b->avail;
        b->size -= count;
    }
    else {
        b->begin = 0;
        b->size = 0;
    }
}

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "buffer.h"


void ep_buffer_init_default(struct ep_buffer *b) {
    ep_buffer_init(b, malloc(EP_BUFFER_DEFAULT_SIZE), EP_BUFFER_DEFAULT_SIZE);
}


void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->size = 0;
}


void ep_buffer_wrap(struct ep_buffer *b, char *buf, size_t count) {
    b->ptr = buf;
    b->avail = count;
    b->begin = 0;
    b->size = count;
}


void ep_buffer_resize(struct ep_buffer *b, size_t newsize) {
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


void ep_buffer_free(struct ep_buffer *b) {
    b->avail = 0;
    b->begin = 0;
    b->size = 0;
    free(b->ptr);
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


void ep_buffer_clear(struct ep_buffer *b) {
    b->begin = 0;
    b->size = 0;
}

size_t ep_buffer_copy(struct ep_buffer *outbuf, struct ep_buffer *inbuf, size_t start) {
    printf("TODO: copy\n");
    return 0;
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


size_t ep_buffer_peek(struct ep_buffer *b, char *outbuf, size_t offset, size_t count) {
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


size_t ep_buffer_erase(struct ep_buffer *b, char *outbuf, size_t count) {
    size_t s = ep_buffer_peek(b, outbuf, 0, count);
    ep_buffer_release(b, s);
    return s;
}


ssize_t ep_buffer_take(struct ep_buffer *b, int fd) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    // try read from file descriptor
    ssize_t r = read(fd, back, space);
    if (r < 0) {
        perror("read");
        printf("%x : %d, %d\n", back, space, b->size);
    }
    else {
        b->size += r;
    }
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

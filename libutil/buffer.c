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


size_t ep_buffer_endmem(struct ep_buffer *b) {
    size_t end = b->begin + b->size;
    if (end < b->avail) {
        return b->avail - end;
    }
    else {
        return b->avail - b->size;
    }
}


ssize_t ep_buffer_insert(struct ep_buffer *b, char *inbuf, size_t count) {
    size_t end = b->begin + b->size;

    // check if insert will exceed capacity
    if (count < (b->avail - b->size)) {
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


ssize_t ep_buffer_take(struct ep_buffer *b, int fd) {
    size_t end = (b->begin + b->size) % b->avail;
    size_t maxread = ep_buffer_endmem(b);
    ssize_t r = read(fd, &b->ptr[end], maxread);
    if (r < 0) {
        return 0;
    }

    // wrap around to beginning
    if (r == maxread && b->begin > 0) {
        size_t rs = read(fd, b->ptr, b->begin);
        if (rs > 0) {
            r += rs;
        }
    }
    b->size += r;
    printf("buffer read %d, (%d / %d used)\n", r, b->size, b->avail);
    return r;
}


ssize_t ep_buffer_write(struct ep_buffer *b, struct ep_dest *d, size_t begin) {

    // write as much as possible from begin
    size_t end = b->begin + b->size;
    if (end < b->avail) {
        return write(d->fd, &b->ptr[begin], end - begin);
    }
    else if (begin < (end - b->avail)) {
        return write(d->fd, &b->ptr[begin], (end - b->avail) - begin);
    }
    else {
        ssize_t w = write(d->fd, &b->ptr[begin], (b->begin - b->avail));
        w += write(d->fd, b->ptr, (end - b->avail));
        return w;
    }
}


void ep_buffer_release(struct ep_buffer *b, size_t count) {
    if (count < b->size) {
        b->begin += count;
        b->size -= count;
    }
    else {
        b->begin = 0;
        b->size = 0;
    }
}


void *ep_thread_read(void *p) {
    struct ep_read_data *rd = (struct ep_read_data *) p;
    while (1) {
        printf("wait for events...\n");

        // wait for events on the socket
        int r = poll(&rd->tbsrc->ksrc, 1, -1);

        // since only one thread writes, no lock is required
        ep_buffer_take(&rd->buf, rd->tbsrc->ksrc.fd);

        // notify of event
        rd->notify_read(rd->tbsrc);
    }
}


void ep_create_reader(struct ep_source *s, notify_fn_t fn) {
    printf("starting endpoint thread\n");

    // data for the read thread
    size_t buf_size = 4096;
    char *mem = malloc(sizeof(struct ep_read_data) + buf_size);
    struct ep_read_data *rd = (struct ep_read_data *) mem;
    rd->tbsrc = s;
    rd->notify_read = fn;
    ep_buffer_init(&rd->buf, &mem[sizeof(struct ep_read_data)], buf_size);
    s->mem = rd;

    int err = pthread_create(&s->thread, NULL, ep_thread_read, s->mem);
    if (err) {
        perror("pthread_create");
    }
    else {
        s->state = 1;
    }
}

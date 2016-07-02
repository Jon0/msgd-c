#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "buffer.h"


void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->end = 0;
}


size_t ep_buffer_used(struct ep_buffer *b) {
    if (b->end < b->begin) {
        return (b->avail + b->end) - b->begin;
    }
    else {
        return b->end - b->begin;
    }
}


size_t ep_buffer_continuous(struct ep_buffer *b) {
    if (b->end < b->begin) {
        return b->begin - b->end;
    }
    else {
        return b->avail - b->end;
    }
}


ssize_t ep_buffer_insert(struct ep_buffer *b, char *inbuf, size_t count) {
    size_t cs = ep_buffer_continuous(b);
    size_t copied = 0;
    char *back = &b->ptr[b->end];
    if (count < cs) {
        memcpy(back, inbuf, count);
        copied = count;
    }
    else {
        memcpy(back, inbuf, cs);
        copied = cs;

        // how much more memory is available?
        size_t memfree = 0;
        if (count - cs < memfree) {
            memcpy(b->ptr, &inbuf[cs], count - cs);
        }

    }

    b->end = (b->end + copied) % b->avail;
    return copied;
}


ssize_t ep_buffer_take(struct ep_buffer *b, int fd) {
    size_t maxread = b->avail - b->end;
    ssize_t r = read(fd, &b->ptr[b->end], maxread);
    if (r > 0) {
        b->end += r;
    }

    // wrap around to beginning
    if (r == maxread && b->begin > 0) {
        size_t rs = read(fd, b->ptr, b->begin);
        if (rs > 0) {
            b->end = rs;
            r += rs;
        }
    }
    printf("buffer read %d, (%d / %d used)\n", r, ep_buffer_used(b), b->avail);
    return r;
}


ssize_t ep_buffer_write(struct ep_buffer *b, struct ep_dest *d, size_t begin) {
    // write as much as possible from begin
    // fix wrap past end of array
    return write(d->fd, &b->ptr[begin], b->end - begin);
}


void ep_buffer_release(struct ep_buffer *b, size_t count) {
    b->begin += count;
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

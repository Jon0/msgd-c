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


ssize_t ep_buffer_insert(struct ep_buffer *b, const char *inbuf, size_t count) {
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


size_t ep_buffer_take_src(struct ep_buffer *b, struct ep_source *s, size_t count) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    if (count > space) {
        count = space;
    }

    ssize_t r = read(s->ksrc.fd, back, count);
    if (r < 0) {
        perror("read");
        return 0;
    }
    else {
        b->size += r;
        return r;
    }
}


ssize_t ep_buffer_write(struct ep_buffer *b, struct ep_dest *d, size_t begin) {

    // write as much as possible from begin
    size_t end = b->begin + b->size;
    if (end < b->avail) {
        return write(d->fd, &b->ptr[begin], end - begin);
    }
    else if (begin < (end - b->avail)) {
        // begin is positioned before end, only one write is required
        return write(d->fd, &b->ptr[begin], (end - b->avail) - begin);
    }
    else {
        // begin is positioned after end, not all can be written
        return write(d->fd, &b->ptr[begin], (b->begin - b->avail));
    }
}


size_t ep_buffer_write_inc(struct ep_buffer *b, struct ep_dest *d, size_t *begin) {
    ssize_t w = ep_buffer_write(b, d, *begin);
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


// replace this function using the channel system
void *ep_thread_read(void *p) {
    struct ep_read_data *rd = (struct ep_read_data *) p;
    struct ep_src *s = rd->srcaddr->src;
    while (1) {
        printf("wait for events...\n");

        // wait for events on the socket
        int r = poll(&s->ksrc, 1, -1);

        // since only one thread writes, no lock is required
        if (ep_buffer_take(&rd->buf, s->ksrc.fd) == 0) {
            printf("socket closed\n");
            return rd;
        }

        // notify of event
        rd->notify_read(rd->srcaddr, rd->notify_obj);
    }
}


void ep_create_reader(struct ep_address *a, notify_fn_t fn, void *obj) {
    printf("starting endpoint thread\n");

    // data for the read thread
    size_t buf_size = 4096;
    char *mem = malloc(sizeof(struct ep_read_data) + buf_size);
    struct ep_read_data *rd = (struct ep_read_data *) mem;
    rd->srcaddr = a;
    rd->notify_obj = obj;
    rd->notify_read = fn;
    ep_buffer_init(&rd->buf, &mem[sizeof(struct ep_read_data)], buf_size);

    // init source struct
    struct ep_src *s = a->src;
    s->mem = rd;
    int err = pthread_create(&s->thread, NULL, ep_thread_read, rd);
    if (err) {
        perror("pthread_create");
    }
    else {
        s->state = 1;
    }
}

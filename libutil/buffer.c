#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"


void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->end = 0;
}


void ep_buffer_insert(struct ep_buffer *b, char *inbuf, size_t count) {
    // ignoring wrap around...
    char *back = &b->ptr[b->end];
    memcpy(back, inbuf, count);
    b->end = (b->end + count) % b->avail;
}


ssize_t ep_buffer_write(const struct ep_buffer *b, int fd, size_t begin) {
    return 0;
}


void ep_buffer_take(struct ep_buffer *b, int fd) {
    ssize_t r = read(fd, &b->ptr[b->end], 1024);
    if (r > 0) {
        b->end += r;
    }
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
    }
}


void ep_create_reader(struct ep_source *s, notify_fn_t fn) {
    printf("starting endpoint thread\n");

    // data for the read thread
    size_t buf_size = 4096;
    char *mem = malloc(sizeof(struct ep_read_data) + buf_size);
    struct ep_read_data *rd = (struct ep_read_data *) mem;
    rd->tbsrc = s;
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


ssize_t ep_write(struct ep_dest *d, struct ep_buffer *b, size_t begin, size_t end) {
    // fix wrap past end of array
    return write(d->fd, &b->ptr[begin], end - begin);
}

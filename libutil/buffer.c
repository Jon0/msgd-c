#include <stdio.h>
#include <unistd.h>

#include "buffer.h"


void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count) {
    b->ptr = mem;
    b->avail = count;
    b->begin = 0;
    b->end = 0;
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

}


void *ep_thread_read(void *p) {
    struct ep_read_data *rd = (struct ep_read_data *) p;
    while (1) {
        // wait for events on the socket
        int r = poll(&rd->event, 1, -1);

        // since only one thread writes, no lock is required
        ep_buffer_take(rd->buf, rd->event.fd);
    }
}


void ep_create_reader(struct ep_source *s, int fd, notify_fn_t fn) {
    printf("creating endpoint reader\n");

}

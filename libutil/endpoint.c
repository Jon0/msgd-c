#include <stdio.h>
#include <unistd.h>

#include "endpoint.h"


void ep_handler_init(struct ep_handler *h, size_t size, ep_callback_t c, void *d) {
    ep_buffer_init(&h->buf, malloc(size), size);
    pthread_mutex_init(&h->mutex, NULL);
    h->callback = c;
    h->min_input = 0;
    h->data = d;
}


void ep_address_print(struct ep_address *a) {
    struct sockaddr_in *s = (struct sockaddr_in *) &a->data;
    char *addr = (char *) &s->sin_addr;
    printf("%d.%d.%d.%d\n", addr[0], addr[1], addr[2], addr[3]);
}


size_t ep_ch_write_blk(struct ep_channel *c, char *b, size_t count) {
    size_t wr = write(c->fd, b, count);
    if (wr < 0) {
        perror("write");
        return 0;
    }
    return wr;
}


size_t ep_hdl_write_blk(struct ep_handler *h, char *b, size_t count) {
    return ep_buffer_insert(&h->buf, b, count);
}

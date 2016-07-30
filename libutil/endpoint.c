#include "endpoint.h"

void ep_handler_init(struct ep_handler *h, size_t size, ep_callback_t c, void *d) {
    ep_buffer_init(&h->buf, malloc(size), size);
    pthread_mutex_init(&h->mutex, NULL);
    h->callback = c;
    h->min_input = 0;
    h->data = d;
}

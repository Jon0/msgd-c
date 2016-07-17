#include "handler.h"


void ep_hdlset_init(struct ep_hdlset *s, size_t max_hdl) {
    s->hdl = malloc(sizeof(struct ep_hdlset *) * max_hdl);
    s->size = 0;
    s->avail = max_hdl;
}


struct ep_handler *ep_handler_create(struct ep_hdlset *s, ep_callback_t c) {
    // allocate memory for buffer and handler
    size_t buffer_size = 4096;
    char *mem = malloc(sizeof(struct ep_handler));
    char *bufmem = &mem[sizeof(struct ep_handler)];

    // init handler values
    struct ep_handler *h = (struct ep_handler *) mem;
    ep_buffer_init(&h->buf, bufmem, buffer_size);
    h->callback = c;
    h->min_input = 0;
    s->hdl[s->size] = h;
    ++s->size;
    return h;
}

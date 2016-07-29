#include <stdio.h>

#include "channel.h"
#include "protocol.h"


int msg_server_accept(struct ep_table *t, int *out) {
    printf("accept\n");
    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_server_recv);
    *out = ep_add_handler(t, &hdl);
    return 1;
}


void msg_server_recv(int ex, struct ep_event_view *ev) {
    char out [256];
    struct ep_buffer *b = &ev->self->buf;
    size_t s = ep_buffer_erase(b, out, 256);
    printf("recv %d: %s\n", s, out);
}


void msg_server_run(struct msg_server_state *s, const char *sockpath) {
    ep_table_init(&s->tb, 256);
    ep_thread_pool_create(&s->pool, &s->tb, 4);

    // create an acceptor
    struct ep_acceptor acc;
    ep_listen_remote(&acc.addr, 2204);
    ep_init_acceptor(&acc, msg_server_accept);
    ep_add_acceptor(&s->tb, &acc);

    // main loop
    ep_queue_from_table(&s->pool.queue);

    // wait until threads complete
    ep_thread_pool_join(&s->pool);
    ep_table_free(&s->tb);
}

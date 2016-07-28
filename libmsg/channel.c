#include <stdio.h>

#include "channel.h"
#include "protocol.h"


void msg_server_run(struct msg_server_state *s, const char *sockpath) {
    ep_table_init(&s->tb, 256);
    ep_thread_pool_create(&s->pool, &s->tb, 4);

    // create an acceptor
    struct ep_acceptor acc;
    ep_listen_remote(&acc.addr, 2204);
    ep_init_acceptor(&acc);
    ep_add_acceptor(&s->tb, &acc);

    // main loop
    ep_queue_from_table(&s->pool.queue);

    // wait until threads complete
    ep_thread_pool_join(&s->pool);
    ep_table_free(&s->tb);
}

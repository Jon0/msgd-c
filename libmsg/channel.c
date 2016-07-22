#include <stdio.h>

#include "channel.h"
#include "protocol.h"


void msg_server_run(struct msg_server_state *s, const char *sockpath) {
    ep_table_init(&s->tb);

    // create a listener
    struct ep_handler *h = ep_new_hdl(&s->tb, NULL);
    struct ep_address *addr = ep_new_addr(&s->tb, h->epid);
    ep_unlink(sockpath);
    ep_set_local(addr, sockpath);
    ep_new_endpoints(&s->tb, addr->epid, AF_UNIX, NULL);


    // wait until threads complete
    ep_thread_pool_join(&s->pool);
    ep_table_free(&s->tb);
}

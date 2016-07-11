#include <stdio.h>

#include "channel.h"
#include "protocol.h"


void channel_thread(struct ep_source *s) {
    struct ep_buffer buf;

    // wait for events
    ep_wait(s);


    // seperate contained streams
    ep_buffer_take_src(&buf, s, 4);
}


void msg_server_run(struct msg_server_state *s, const char *sockpath) {
    ep_table_init(&s->tb);

    // create a listener
    struct ep_address *addr = ep_new_addr(&s->tb);
    ep_unlink(sockpath);
    ep_set_local(addr, sockpath);
    ep_add_pipe_endpoints(&s->tb, addr->epid);


    // wait until threads complete
    ep_pool_join(&s->pool);
    ep_table_free(&s->tb);
}

#include <stdio.h>

#include "channel.h"
#include "protocol.h"

void on_accept(struct ep_address *a, void *p) {
    struct msg_server_state *ss = (struct msg_server_state *) p;
    printf("new connection accepted\n");

    // initialise a new channel

}


void on_read(struct ep_address *a, void *p) {
    struct msg_server_state *ss = (struct msg_server_state *) p;
    struct ep_read_data *read = (struct ep_read_data *) a->src->mem;
    msg_poll_buffer(&read->buf);
}


void channel_thread(struct ep_source *s) {
    // a thread for handle socket input

    // wait for events
    ep_wait(s);
}


void msg_server_run(struct msg_server_state *s, const char *sockpath) {
    ep_table_init(&s->tb, "");

    // create a listener
    struct ep_address *addr = ep_new_addr(&s->tb);
    ep_unlink(sockpath);
    ep_set_local(addr, sockpath);
    ep_add_pipe_endpoints(&s->tb, addr->epid);
    ep_activate_acceptor(&s->tb, addr->epid, on_accept, on_read, s);

    // wait until threads complete
    ep_table_join(&s->tb);
    ep_table_free(&s->tb);
}

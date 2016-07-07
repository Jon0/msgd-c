#include <stdio.h>

#include "channel.h"


void on_accept(struct ep_address *a, void *p) {
    struct msg_channel_table *t = (struct msg_channel_table *) p;
    printf("accept notify\n");

    // initialise a new channel

}


void on_read(struct ep_address *a, void *p) {
    struct msg_channel_table *t = (struct msg_channel_table *) p;
    struct ep_read_data *read = (struct ep_read_data *) a->src->mem;
    msg_parse_input(&read->buf);
}


void msg_parse_input(struct ep_buffer *b) {
    char *content = &b->ptr[b->begin];

    // parse the buffer content
    printf("read %s\n", content);

    // recieving requests to the local server
}


void msg_server_run(struct msg_server_state *s) {
    ep_table_init(&s->tb, "");

    // create a listener
    struct ep_address *addr = ep_new_addr(&s->tb);
    ep_set_local(addr, "msgd-local");
    ep_add_pipe_endpoints(&s->tb, addr->epid);
    ep_activate_acceptor(&s->tb, addr->epid, on_accept, on_read, &s->c);

    // wait until threads complete
    ep_table_join(&s->tb);
    ep_table_free(&s->tb);
}

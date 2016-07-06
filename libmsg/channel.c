#include <stdio.h>

#include "channel.h"


void *on_accept(struct ep_source *s) {
    printf("accept notify\n");
    return NULL;
}


void *on_read(struct ep_source *s) {
    struct ep_read_data *read = (struct ep_read_data *) s->mem;
    msg_parse_input(&read->buf);
    return NULL;
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
    ep_activate_acceptor(&s->tb, addr->epid, on_accept, on_read);

    // wait until threads complete
    ep_table_join(&s->tb);
    ep_table_free(&s->tb);
}

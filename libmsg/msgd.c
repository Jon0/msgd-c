#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>

#include "msgd.h"
#include "protocol.h"


void on_client_read(struct ep_address *a, void *p) {
    printf("read notify\n");
}


void write_str(struct msg_client_state *cs, const char *str) {
    msg_push_buffer(&cs->buf, str, strlen(str));
    size_t r = ep_write_buf(&cs->pool.queue, cs->epid, &cs->buf);
}


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode) {
    ep_table_init(&cs->tb, 256);
    cs->writepos = 0;

    size_t bufsize = 4096;
    ep_buffer_init(&cs->buf, malloc(bufsize), bufsize);

    struct ep_channel ch;
    ep_connect_remote(&ch.addr, "127.0.0.1", 2204);
    ep_init_channel(&ch);
    ep_add_channel(&cs->tb, &ch);


    // send connect request
    write_str(cs, "connect");
}


void msg_free_proc(struct msg_client_state *cs) {

    // wait until threads complete
    ep_thread_pool_join(&cs->pool);
    ep_table_free(&cs->tb);
    free(cs->buf.ptr);
}


void msg_publish(struct msg_client_state *cs, const struct node_attr_set *ns) {

}


void msg_subscribe(struct msg_client_state *cs, const struct node_attr_set *ns) {

}


void msg_available(struct msg_client_state *cs, struct node_id_set *ns) {
    write_str(cs, "available");
}


void msg_published(struct msg_client_state *cs, struct node_id_set *ns) {

}

void msg_subscribed(struct msg_client_state *cs, struct node_id_set *ns) {

}


void msg_poll(struct msg_client_state *cs) {}
void msg_push(struct msg_client_state *cs) {}

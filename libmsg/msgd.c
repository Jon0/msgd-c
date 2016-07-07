#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>

#include "msgd.h"


void on_client_read(struct ep_address *a, void *p) {
    printf("read notify\n");
}


void write_str(struct msg_client_state *cs, const char *str) {
    ep_buffer_insert(&cs->buf, str, strlen(str));
    struct ep_dest *d = ep_table_dest(&cs->tb, cs->epid);
    size_t r = ep_buffer_write_inc(&cs->buf, d, &cs->writes);
}


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode) {
    ep_table_init(&cs->tb, "");
    cs->writes = 0;

    size_t bufsize = 4096;
    ep_buffer_init(&cs->buf, malloc(bufsize), bufsize);

    // create a connector
    struct ep_address *a = ep_new_addr(&cs->tb);
    cs->epid = a->epid;
    ep_set_local(a, "msgd-local");
    ep_add_pipe_endpoints(&cs->tb, cs->epid);
    ep_activate_connector(a, on_client_read, NULL);
    strcpy(cs->proc_name, name);

    // send connect request
    write_str(cs, "connect");
}


void msg_free_proc(struct msg_client_state *cs) {

    // wait until threads complete
    ep_table_join(&cs->tb);
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

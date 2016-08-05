#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>

#include "msgd.h"
#include "protocol.h"


void msg_client_recv(int ex, struct ep_event_view *ev) {
    printf("recv\n");
}


void msg_connect(struct msg_client_state *cs, const char *addr, short port) {
    ep_table_init(&cs->tb, 256);
    ep_thread_pool_create(&cs->pool, &cs->tb, 4);
    cs->writepos = 0;

    size_t bufsize = 4096;
    ep_buffer_init(&cs->buf, malloc(bufsize), bufsize);

    struct ep_channel ch;
    ep_connect_remote(&ch.addr, addr, port);
    int err = ep_init_channel(&ch);
    if (err == -1) {
        return;
    }
    cs->epid = ep_add_channel(&cs->tb, &ch);
    ep_sink_init(&cs->pool.queue, cs->epid, &cs->out);

    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_client_recv, NULL);
    cs->hdlid = ep_add_handler(&cs->tb, &hdl);
    ep_table_ctl(&cs->tb, cs->epid, cs->hdlid);
}


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode) {

    // send connect request
    msg_req_addproc(&cs->buf, name, strlen(name));
    cs->writepos = ep_write_buf(&cs->out, &cs->buf, cs->writepos);
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
    msg_req_avail(&cs->buf);
    cs->writepos = ep_write_buf(&cs->out, &cs->buf, cs->writepos);
    int id;
    int r = ep_table_wait(&cs->tb, &id, 1);
    struct ep_event ev;
    ev.epid = id;
    ev.srcid = 0;
    if (r) {
        printf("recv reply %d\n", id);
        ep_queue_update(&cs->pool.queue, &ev);
    }
}


void msg_published(struct msg_client_state *cs, struct node_id_set *ns) {

}

void msg_subscribed(struct msg_client_state *cs, struct node_id_set *ns) {

}


void msg_poll(struct msg_client_state *cs) {}
void msg_push(struct msg_client_state *cs) {}

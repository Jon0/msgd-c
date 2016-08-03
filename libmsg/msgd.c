#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>

#include "msgd.h"
#include "protocol.h"


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode) {
    ep_table_init(&cs->tb, 256);
    ep_thread_pool_create(&cs->pool, &cs->tb, 4);
    cs->writepos = 0;

    size_t bufsize = 4096;
    ep_buffer_init(&cs->buf, malloc(bufsize), bufsize);

    struct ep_channel ch;
    ep_connect_remote(&ch.addr, "127.0.0.1", 2204);
    int err = ep_init_channel(&ch);
    if (err == -1) {
        return;
    }
    cs->epid = ep_add_channel(&cs->tb, &ch);


    // send connect request
    msg_req_addproc(&cs->buf, name, strlen(name));
    struct ep_event ev;
    ev.epid = cs->epid;
    ev.srcid = 0;
    cs->writepos = ep_queue_wbuf(&cs->pool.queue, &ev, &cs->buf, cs->writepos);
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
    struct ep_event ev;
    ev.epid = cs->epid;
    ev.srcid = 0;
    cs->writepos = ep_queue_wbuf(&cs->pool.queue, &ev, &cs->buf, cs->writepos);
}


void msg_published(struct msg_client_state *cs, struct node_id_set *ns) {

}

void msg_subscribed(struct msg_client_state *cs, struct node_id_set *ns) {

}


void msg_poll(struct msg_client_state *cs) {}
void msg_push(struct msg_client_state *cs) {}

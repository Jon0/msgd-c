#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>
#include <libutil/tree.h>

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
    ep_buffer_init(&cs->send_buf, malloc(bufsize), bufsize);
    ep_buffer_init(&cs->recv_buf, malloc(bufsize), bufsize);

    struct ep_channel ch;
    ep_connect_remote(&ch.addr, addr, port);
    int err = ep_init_channel(&ch);
    if (err == -1) {
        cs->connected = 0;
        return;
    }
    cs->epid = ep_add_channel(&cs->tb, &ch);
    ep_sink_init(&cs->pool.queue, cs->epid, &cs->out);

    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_client_recv, NULL);
    cs->hdlid = ep_add_handler(&cs->tb, &hdl);
    ep_table_ctl(&cs->tb, cs->epid, cs->hdlid);
    cs->connected = 1;
}


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode) {
    if (cs->connected) {

        // send connect request
        msg_req_addproc(&cs->send_buf, name, strlen(name));
        cs->writepos = ep_write_buf(&cs->out, &cs->send_buf, cs->writepos);

        // hostname will be returned
        printf("wait for tree state\n");
        ep_table_read_buf(&cs->tb, cs->epid, &cs->recv_buf);

        // init tree
        msg_tree_init(&cs->tree);
        ep_tree_read(&cs->tree, &cs->recv_buf);
        ep_tree_print(&cs->tree);
    }
    else {
        printf("no connection\n");
    }
}


void msg_free_proc(struct msg_client_state *cs) {

    // wait until threads complete
    ep_thread_pool_join(&cs->pool);
    ep_table_free(&cs->tb);
    free(cs->send_buf.ptr);
    free(cs->recv_buf.ptr);
}


void msg_publish(struct msg_client_state *cs, const struct node_attr_set *ns) {

}


void msg_subscribe(struct msg_client_state *cs, const struct node_attr_set *ns) {

}


int msg_available(struct msg_client_state *cs, struct msg_node_set *ns) {
    if (!cs->connected) {
        printf("no connection\n");
        return 0;
    }


    msg_req_avail(&cs->send_buf, &cs->tree);
    cs->writepos = ep_write_buf(&cs->out, &cs->send_buf, cs->writepos);

    // wait for reply
    ep_table_read_buf(&cs->tb, cs->epid, &cs->recv_buf);
    ep_tree_read(&cs->tree, &cs->recv_buf);
    ep_tree_print(&cs->tree);
    return 0;
}


void msg_published(struct msg_client_state *cs, struct msg_node_set *ns) {

}

void msg_subscribed(struct msg_client_state *cs, struct msg_node_set *ns) {

}


void msg_poll(struct msg_client_state *cs) {}
void msg_push(struct msg_client_state *cs) {}

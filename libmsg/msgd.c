#include <stdio.h>
#include <string.h>

#include <libutil/socket.h>
#include <libutil/tree.h>

#include "msgd.h"
#include "protocol.h"


void msg_client_recv(int ex, struct ep_event_view *ev) {
    struct msg_client_state *cs = (struct msg_client_state *) ev->self->data;
    struct ep_buffer *recv_buf = &ev->self->buf;
    struct ep_table_entry *e = ep_map_get(&cs->tb.entries, ev->src);
    if (e) {
        printf("recv msg length: %d\n", recv_buf->size);

        // read tree state
        //msg_poll_apply(&cs->tree, &req);
        while (ep_tree_read(&cs->tree, recv_buf) == 0) {}

        printf("\ncurrent tree state:\n");
        ep_tree_print(&cs->tree);
        msg_tree_elems(&cs->tree);
    }
}


int msg_wait(struct msg_client_state *cs, int type) {

}


int msg_connect(struct msg_client_state *cs, const char *addr, short port) {
    ep_table_init(&cs->tb, 256);
    ep_thread_pool_create(&cs->pool, &cs->tb, 1, EP_EPOLL);

    struct ep_channel ch;
    ep_connect_remote(&ch.addr, addr, port);
    int err = ep_init_channel(&ch);
    if (err == -1) {
        cs->connected = 0;
        return err;
    }
    cs->server_id = ep_add_channel(&cs->tb, &ch);

    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_client_recv, cs);
    cs->hdlid = ep_add_handler(&cs->tb, &hdl);
    ep_table_route(&cs->tb, cs->server_id, cs->hdlid);
    cs->connected = 1;
    msg_tree_init(&cs->tree);
    return 0;
}


int msg_get_peers(struct msg_client_state *cs) {
    if (cs->connected) {

        // send peer request
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_peers(&ch->write_buf);
        printf("sent msg length: %d\n", ch->write_buf.size);
        ep_channel_flush(ch);
    }
    else {
        printf("no connection\n");
    }
}


void msg_create_node(struct msg_client_state *cs, const char *name, int mode) {
    if (cs->connected) {

        // send connect request
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_addproc(&ch->write_buf, name, strlen(name));
        printf("sent msg length: %d\n", ch->write_buf.size);
        ep_channel_flush(ch);
        printf("wait for reply\n");
    }
    else {
        printf("no connection\n");
    }
}


void msg_free_proc(struct msg_client_state *cs) {

    // wait until threads complete
    ep_thread_pool_join(&cs->pool);
    ep_table_free(&cs->tb);
}


int msg_publish(struct msg_client_state *cs, const char *name, int nodeid) {
    if (cs->connected) {

        // send connect request
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_publish(&ch->write_buf, name, strlen(name), nodeid);
        printf("sent msg length: %d\n", ch->write_buf.size);
        ep_channel_flush(ch);
        printf("wait for reply\n");
        // TODO create handler to send updates back to server
        return -1;
    }
    else {
        printf("no connection\n");
        return -1;
    }
}


void msg_subscribe(struct msg_client_state *cs, int nodeid, int subid) {
    if (cs->connected) {

        // send subscribe request
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_subscribe(&ch->write_buf, nodeid, subid);
        printf("sent msg length: %d\n", ch->write_buf.size);
        ep_channel_flush(ch);
        printf("wait for reply\n");
    }
    else {
        printf("no connection\n");
    }
}


int msg_available(struct msg_client_state *cs, struct msg_node_set *ns) {
    if (!cs->connected) {
        printf("no connection\n");
        return 0;
    }

    struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
    struct ep_channel *ch = &e->data.ch;
    msg_req_avail(&ch->write_buf, &cs->tree);
    printf("sent msg length: %d\n", ch->write_buf.size);
    ep_channel_flush(ch);
    printf("wait for reply\n");
    return 0;
}


void msg_published(struct msg_client_state *cs, struct msg_node_set *ns) {

}

void msg_subscribed(struct msg_client_state *cs, struct msg_node_set *ns) {

}


void msg_poll(struct msg_client_state *cs) {}
void msg_push(struct msg_client_state *cs) {}

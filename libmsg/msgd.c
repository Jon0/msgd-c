#include <stdio.h>
#include <string.h>

#include <libutil/tree.h>
#include <libsys/socket.h>

#include "msgd.h"


void msg_client_recv(int ex, struct ep_event_view *ev) {
    struct msg_client_state *cs = (struct msg_client_state *) ev->self->data;
    struct ep_buffer *recv_buf = &ev->self->buf;
    struct ep_table_entry *e = ep_map_get(&cs->tb.entries, ev->src);
    struct msg_host *server = msg_client_host(cs);
    struct msg_message msg;
    if (e) {
        printf("recv msg length: %lu\n", recv_buf->size);
        while(msg_poll_message(recv_buf, &msg)) {
            msg_client_apply(cs, ev->src, &msg);
            ep_buffer_release(msg.body, msg.head.size);
        }
        msg_host_list_debug(&cs->hosts);
    }
}


struct msg_host *msg_client_host(struct msg_client_state *cs) {
    return &cs->hosts.ptr[0];
}


int msg_client_apply(struct msg_client_state *cs, int srcid, struct msg_message *msg) {
    struct msg_host *server = msg_client_host(cs);
    int read_size;


    // TODO queue events to be polled
    printf("recv type %d (%d bytes)\n", msg->head.type, msg->head.size);
    switch (msg->head.type) {
    case msg_type_peer_one:
    case msg_type_peer_all:
        msg_merge_peers(&cs->hosts, msg->body, 0);
        break;
    case msg_type_data:
        printf("recv data\n");
        break;
    }
}


int msg_connect(struct msg_client_state *cs, struct ep_address *addr) {
    ep_table_init(&cs->tb, 256);
    ep_thread_pool_create(&cs->pool, &cs->tb, 1, EP_EPOLL);
    ep_multimap_init(&cs->node_to_hdl, sizeof(int), 1024);

    struct ep_channel ch;
    ch.addr = *addr;
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

    // init host memory
    msg_host_list_init(&cs->hosts, 32, 0);
    return 0;
}


void msg_register_proc(struct msg_client_state *cs, const char *name, int mode) {
    if (cs->connected) {

        // send connect request
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_proc_init(&ch->write_buf, name, strlen(name));
        printf("sent msg length: %lu\n", ch->write_buf.size);
        ep_channel_flush(ch);
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


int msg_create_share(struct msg_client_state *cs, const char *path) {
    if (cs->connected) {
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_req_share(&ch->write_buf, path);
        printf("sent msg length: %lu\n", ch->write_buf.size);
        ep_channel_flush(ch);
    }
    else {
        printf("no connection\n");
    }
}


size_t msg_write(struct msg_client_state *cs, int nodeid, int hdlid, char *buf, size_t count) {
    if (cs->connected) {
        struct ep_table_entry *e = ep_map_get(&cs->tb.entries, cs->server_id);
        struct ep_channel *ch = &e->data.ch;
        msg_send_block(&ch->write_buf, nodeid, hdlid, buf, count);
        printf("write length: %lu\n", ch->write_buf.size);
        ep_channel_flush(ch);
    }
    else {
        printf("no connection\n");
    }
    return count;
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

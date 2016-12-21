#include <stdio.h>
#include <string.h>

#include "msgd.h"


void msg_client_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_client_state *cs = p;
}


static struct msgu_handlers msg_client_handlers = {
    .connect_event = msg_client_connect_event,
};


void msg_client_socket_recv(void *p, struct msgu_recv_event *recv) {
    struct msg_client_state *cs = p;
    struct msgu_buffer *recv_buf = recv->buf;
    struct msg_host *server = msg_client_host(cs);
    struct msg_message msg;
    printf("recv msg length: %lu\n", recv_buf->size);
    while(msg_poll_message(recv_buf, &msg)) {
        msg_client_apply(cs, recv->id, &msg);
        ep_buffer_release(msg.body, msg.head.size);
    }
    msg_host_list_debug(&cs->hosts);
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


int msg_connect(struct msg_client_state *cs, struct msgu_address *addr) {
    msgu_event_map_init(&cs->emap, &msg_client_handlers, cs);
    msgu_multimap_init(&cs->node_to_hdl, sizeof(int), 1024);
    msgu_buffer_init(&cs->write_buf, malloc(1024), 1024);
    msgs_table_init(&cs->tb, &cs->emap);


    msgs_open_socket(&cs->server, addr);
    cs->server_id = msgs_poll_socket(&cs->tb, &cs->server);
    cs->connected = 1;


    // init host memory
    msg_host_list_init(&cs->hosts);
    msg_host_list_alloc(&cs->hosts, 32);
    return 0;
}


void msg_register_proc(struct msg_client_state *cs, const char *name, int mode) {
    if (cs->connected) {

        // send connect request
        msg_req_proc_init(&cs->write_buf, name, strlen(name));
        printf("sent msg length: %lu\n", cs->write_buf.size);
    }
    else {
        printf("no connection\n");
    }
}


void msg_free_proc(struct msg_client_state *cs) {
    msgs_table_free(&cs->tb);
}


int msg_create_share(struct msg_client_state *cs, const char *path) {
    if (cs->connected) {
        msg_req_share(&cs->write_buf, path);
        msgs_send(&cs->write_buf, cs->server.fd, cs->write_buf.begin);
        printf("sent msg length: %lu\n", cs->write_buf.size);
    }
    else {
        printf("no connection\n");
    }
}


size_t msg_write(struct msg_client_state *cs, int nodeid, int hdlid, char *buf, size_t count) {
    if (cs->connected) {
        msg_send_block(&cs->write_buf, nodeid, hdlid, buf, count);
        printf("write length: %lu\n", cs->write_buf.size);
    }
    else {
        printf("no connection\n");
    }
    return count;
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

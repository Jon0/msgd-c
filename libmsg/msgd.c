#include <stdio.h>
#include <string.h>

#include "msgd.h"


void msg_client_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_client_state *cs = p;
}


void msg_client_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_client_state *cs = p;
    struct msg_host *server = msg_client_host(cs);
    int read_size;

    // TODO queue events to be polled
    printf("client recv message!\n");
}


static struct msgu_handlers msg_client_handlers = {
    .connect_event = msg_client_connect_event,
    .recv_event = msg_client_recv_event,
};


struct msg_host *msg_client_host(struct msg_client_state *cs) {
    return &cs->hosts.ptr[0];
}


int msg_connect(struct msg_client_state *cs, struct msgu_address *addr) {
    msgu_event_map_init(&cs->emap, &msg_client_handlers, cs);
    msgu_multimap_init(&cs->node_to_hdl, sizeof(int), 1024);
    msgu_buffer_init(&cs->write_buf, malloc(1024), 1024);
    msgs_table_init(&cs->tb, &cs->emap);

    // open socket to server
    msgs_open_socket(&cs->server, addr);
    cs->server_id = msgs_poll_socket(&cs->tb, &cs->server);
    msgu_stream_init(&cs->stream, (msgu_stream_id_t) cs->server.fd, &msgs_socket_fn);
    cs->connected = 1;

    // init host memory
    msg_host_list_init(&cs->hosts);
    msg_host_list_alloc(&cs->hosts, 32);
    return 0;
}


int msg_disconnect(struct msg_client_state *cs) {
    cs->connected = 0;
    msgs_close_socket(&cs->server);
    msgs_table_free(&cs->tb);
}


int msg_send_message(struct msg_client_state *cs, int type, union msgu_any_update *u) {
    struct msgu_channel_header head;
    struct msgu_fragment f[8];
    if (cs->connected) {
        head.data_type = type;
        head.size = msgu_update_size(type, u);
        msgu_fragment_reset(f, 8);
        msgu_update_print(type, u);
        return msgu_push_update(&cs->stream, f, &head, u);
    }
    else {
        printf("no connection\n");
        return -1;
    }
}


int msg_init_local(struct msg_client_state *cs) {
    struct msgu_init_local_update init;
    init.version_maj = 0;
    init.version_min = 1;
    return msg_send_message(cs, msgtype_init_local, (union msgu_any_update *) &init);
}


int msg_list_shares(struct msg_client_state *cs) {
    struct msgu_empty_update listshare;
    return msg_send_message(cs, msgtype_list_shares, (union msgu_any_update *) &listshare);
}


int msg_create_share(struct msg_client_state *cs, char *path) {
    struct msgu_share_file_update addshare;
    addshare.share_name.count = strlen(path);
    addshare.share_name.buf = path;
    return msg_send_message(cs, msgtype_add_share_file, (union msgu_any_update *) &addshare);
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

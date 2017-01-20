#include <stdio.h>
#include <string.h>

#include "msgd.h"


void msg_client_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_client_state *cs = p;
}


void msg_client_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_client_state *cs = p;
    union msgu_any_update data;
    int data_type;

    // TODO queue events to be polled
    while (msgu_channel_read(&cs->channel)) {
        if (msgu_channel_update_move(&cs->channel, &data_type, &data)) {
            msgu_update_print(data_type, &data);
            msgu_update_free(data_type, &data);
        }
        else {
            printf("update: move failed\n");
        }
    }
    if (msgu_channel_is_closed(&cs->channel)) {
        printf("connection closed\n", e->id);
    }
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
    if (msgs_open_socket(&cs->server, addr)) {
        cs->connected = 1;
        cs->server_id = msgu_add_recv_handler(&cs->emap);
        msgu_channel_init(&cs->channel, (msgu_stream_id_t) cs->server.fd, &msgs_socket_fn);
        msgs_poll_socket(&cs->tb, &cs->server, cs->server_id);
    }
    else {
        cs->connected = 0;
    }


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
    if (cs->connected) {
        msgu_update_print(type, u);
        msgu_channel_update_send(&cs->channel, type, u);
        return msgu_channel_write(&cs->channel);
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
    msgu_string_from_static(&addshare.share_name, path);
    return msg_send_message(cs, msgtype_add_share_file, (union msgu_any_update *) &addshare);
}


int msg_open_share(struct msg_client_state *cs, char *path) {
    struct msgu_share_file_update addshare;
    msgu_string_from_static(&addshare.share_name, path);
    return msg_send_message(cs, msgtype_file_open, (union msgu_any_update *) &addshare);
}


int msg_read(struct msg_client_state *cs, int hdl, size_t count) {
    struct msgu_node_read_update handle;
    handle.node_handle = hdl;
    handle.count = count;
    return msg_send_message(cs, msgtype_file_stream_read, (union msgu_any_update *) &handle);
}


int msg_write(struct msg_client_state *cs, int hdl, const char *buf, size_t count) {
    struct msgu_node_write_update stream;
    stream.node_handle = hdl;
    msgu_string_from_buffer(&stream.data, buf, count);
    return msg_send_message(cs, msgtype_file_stream_write, (union msgu_any_update *) &stream);
}


int msg_wait(struct msg_client_state *cs) {
    msgs_table_poll_one(&cs->tb);
    return 1;
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

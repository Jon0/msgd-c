#include <stdio.h>
#include <string.h>

#include "msgd.h"


void msg_client_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_client_state *cs = p;
}


void msg_client_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_client_state *cs = p;
    if (e->id == cs->server_id) {
        msg_connection_notify(&cs->server);
        int result = msg_connection_poll(&cs->server);
        if (result == -1) {
            printf("connection %d closed\n", e->id);
        }
    }
}


static struct msgu_handlers msg_client_handlers = {
    .connect_event = msg_client_connect_event,
    .recv_event = msg_client_recv_event,
};


int msg_client_message_recv(struct msg_connection *conn, struct msgu_message *msg, void *cs) {
    msg_connection_log(conn, msg, "recv");
    msgu_message_free(msg);
}


int msg_connect(struct msg_client_state *cs, struct msgu_address *addr) {
    msgu_event_map_init(&cs->emap, &msg_client_handlers, cs);
    msgs_table_init(&cs->tb, &cs->emap);

    cs->server_id = msgu_add_recv_handler(&cs->emap);
    msg_connection_connect(&cs->server, addr, msg_client_message_recv, cs);
    msgs_poll_socket(&cs->tb, &cs->server.socket, cs->server_id);
    return 0;
}


int msg_disconnect(struct msg_client_state *cs) {
    msg_connection_close(&cs->server);
    msgs_table_free(&cs->tb);
}


int msg_init_local(struct msg_client_state *cs, const char *name) {
    struct msgu_init_local_msg init;
    init.version_maj = 0;
    init.version_min = 1;
    msgu_string_from_static(&init.proc_name, name);
    return msg_connection_send_message(&cs->server, msgtype_init_local, msgdata_init_local, (union msgu_any_msg *) &init);
}


int msg_peer_connect(struct msg_client_state *cs, const char *addr) {
    struct msgu_host_addr_msg haddr;
    msgu_string_from_static(&haddr.address_str, addr);
    return msg_connection_send_message(&cs->server, msgtype_peer_connect, msgdata_host_addr, (union msgu_any_msg *) &haddr);
}


int msg_list_shares(struct msg_client_state *cs) {
    struct msgu_empty_msg listshare;
    return msg_connection_send_message(&cs->server, msgtype_list_shares, msgdata_empty, (union msgu_any_msg *) &listshare);
}


int msg_create_share(struct msg_client_state *cs, char *path) {
    struct msgu_share_file_msg addshare;
    msgu_string_from_static(&addshare.share_name, path);
    return msg_connection_send_message(&cs->server, msgtype_add_share_file, msgdata_share_file, (union msgu_any_msg *) &addshare);
}


int msg_open_share(struct msg_client_state *cs, char *path) {
    struct msgu_share_file_msg addshare;
    msgu_string_from_static(&addshare.share_name, path);
    return msg_connection_send_message(&cs->server, msgtype_file_open, msgdata_share_file, (union msgu_any_msg *) &addshare);
}


int msg_read(struct msg_client_state *cs, int hdl, size_t count) {
    struct msgu_node_read_msg handle;
    handle.node_handle = hdl;
    handle.count = count;
    return msg_connection_send_message(&cs->server, msgtype_file_stream_read, msgdata_node_read, (union msgu_any_msg *) &handle);
}


int msg_write(struct msg_client_state *cs, int hdl, const char *buf, size_t count) {
    struct msgu_node_write_msg stream;
    stream.node_handle = hdl;
    msgu_string_from_buffer(&stream.data, buf, count);
    return msg_connection_send_message(&cs->server, msgtype_file_stream_write, msgdata_node_write, (union msgu_any_msg *) &stream);
}


int msg_wait(struct msg_client_state *cs) {
    msgs_table_poll_one(&cs->tb);
    return 1;
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

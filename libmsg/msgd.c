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
    msgu_stream_init(&cs->stream, cs->server.fd, &msgs_socket_fn);
    cs->connected = 1;

    // init host memory
    msg_host_list_init(&cs->hosts);
    msg_host_list_alloc(&cs->hosts, 32);
    return 0;
}


void msg_register_proc(struct msg_client_state *cs, const char *name, int mode) {
    if (cs->connected) {

        // send connect request
        msg_req_proc_init(&cs->stream, name, strlen(name));
        printf("sent msg length: %lu\n", cs->write_buf.size);
    }
    else {
        printf("no connection\n");
    }
}


void msg_free_proc(struct msg_client_state *cs) {
    msgs_table_free(&cs->tb);
}


int msg_init_local(struct msg_client_state *cs) {
    struct msgu_init_local_update init;
    init.version_maj = 0;
    init.version_min = 1;
    union msgu_any_update *update = (union msgu_any_update *) &init;
    struct msgu_fragment f;
    f.progress = 0;
    if (cs->connected) {
        msgu_update_print(msgtype_init_local, update);
        int result = msgu_push_update(&cs->stream, &f, msgtype_init_local, update);
        return 0;
    }
    else {
        printf("no connection\n");
        return -1;
    }
}


int msg_create_share(struct msg_client_state *cs, char *path) {
    struct msgu_add_share_update addshare;
    addshare.share_name.count = strlen(path);
    addshare.share_name.buf = path;
    union msgu_any_update *update = (union msgu_any_update *) &addshare;
    struct msgu_fragment f;
    f.progress = 0;
    if (cs->connected) {
        msgu_update_print(msgtype_add_share, update);
        int result = msgu_push_update(&cs->stream, &f, msgtype_add_share, update);
        return 0;
    }
    else {
        printf("no connection\n");
        return -1;
    }
}


void msg_poll(struct msg_client_state *cs, struct msg_client_update *up) {

}

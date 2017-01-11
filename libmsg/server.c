#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *serv = p;
    struct msgs_socket newsocket;

    // which acceptor was updated?
    if (e->id == serv->local_id) {
        while (msgs_accept_socket(&serv->local_acc, &newsocket)) {
            printf("server accept: %d (local)\n", e->id);
            msg_server_init_connection(serv, &newsocket);
        }
    }
    else {
        printf("server accept: %d (unknown)\n", e->id);
    }
}


void msg_server_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_server *serv = p;

    // find connection by id
    struct msg_connection *conn = msg_server_connection(serv, e->id);
    struct msg_delta delta;
    if (conn) {
        delta.source_id = e->id;
        delta.source = conn;
        while (msgu_channel_read(&conn->ch)) {
            if (msgu_channel_update_move(&conn->ch, &delta.update_type, &delta.update)) {
                msgu_update_print(delta.update_type, &delta.update);
                msg_server_apply(serv, &delta);
                msgu_update_free(delta.update_type, &delta.update);
                msg_server_print_state(serv);
            }
            else {
                printf("update: move failed\n");
            }
        }
        if (msgu_channel_is_closed(&conn->ch)) {
            // socket was closed
            printf("connection %d: closed\n", e->id);
            msg_server_close_connection(serv, e->id);
        }
    }
    else {
        printf("connection %d: not found\n", e->id);
    }
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event    = msg_server_connect_event,
    .recv_event       = msg_server_recv_event,
};


struct msg_host *msg_server_self(struct msg_server *s) {
    return &s->hosts.ptr[0];
}


int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket) {
    struct msg_connection conn;
    conn.socket = *socket;
    msgu_channel_init(&conn.ch, socket->fd, &msgs_socket_fn);
    int id = msgs_poll_socket(&s->tb, socket);
    msgu_map_insert(&s->connections, &id, &conn);
    return id;
}


int msg_server_close_connection(struct msg_server *s, int id) {

}


struct msg_connection *msg_server_connection(struct msg_server *s, int id) {
    return msgu_map_get(&s->connections, &id);
}


void msg_server_print_state(struct msg_server *serv) {
    printf("server state:\n");
    msg_host_list_debug(&serv->hosts);
}


void msg_server_init(struct msg_server *s, const char *sockpath) {
    msgu_event_map_init(&s->emap, &msg_server_handlers, s);
    msgu_map_init(&s->connections, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct msg_connection));
    msgu_map_alloc(&s->connections, 1024);
    msgu_share_set_init(&s->shares);
    msgs_table_init(&s->tb, &s->emap);

    // find own address and hostname
    struct ep_host host;
    ep_host_init_self(&host);
    msg_host_list_init(&s->hosts);
    msg_host_list_alloc(&s->hosts, 32);
    msg_host_list_add(&s->hosts, host.addr, host.hostname);

    // create a local acceptor
    struct msgu_address local_addr;
    ep_unlink("msgd-ipc");
    ep_local(&local_addr, "msgd-ipc");
    msgs_open_acceptor(&s->local_acc, &local_addr);
    s->local_id = msgs_poll_acceptor(&s->tb, &s->local_acc);

    // create a remote acceptor
    struct msgu_address raddr;
    ep_listen_remote(&raddr, 2204);
    msgs_open_acceptor(&s->remote_acc, &raddr);
    msgs_poll_acceptor(&s->tb, &s->remote_acc);
}


int msg_server_connect(struct msg_server *serv, const char *addr) {
    struct msgu_address raddr;
    struct msgs_socket socket;

    ep_connect_remote(&raddr, addr, 2204);
    int fd = msgs_open_socket(&socket, &raddr);
    if (fd == -1) {
        return fd;
    }
    int cid = msg_server_init_connection(serv, &socket);
    struct msg_connection *conn = msg_server_connection(serv, cid);

    // request table of known addresses
    return cid;
}


void msg_server_run(struct msg_server *serv) {

    // waits until threads complete
    msgs_table_start(&serv->tb, 4);
    msgs_table_free(&serv->tb);
}


int msg_server_apply(struct msg_server *serv, const struct msg_delta *delta) {
    struct msg_status status;
    if (msg_server_validate(serv, delta)) {
        if (msg_server_modify(serv, delta, &status)) {
            msg_server_notify(serv, delta, &status);
            msg_server_reply(serv, delta, &status);
        }
        else {
            return -1;
        }
    }
    return 0;
}


int msg_server_validate(struct msg_server *serv, const struct msg_delta *delta) {
    // check update is valid
    return 1;
}


int msg_server_modify(struct msg_server *serv, const struct msg_delta *delta, struct msg_status *status) {
    struct msgu_file_event fevent;
    char path [256];
    int i;

    // lock mutex and apply state changes
    switch (delta->update_type) {
    case msgtype_init_local:
        printf("updating: init local connection\n");
        break;
    case msgtype_init_remote:
        printf("updating: init remote connection\n");
        break;
    case msgtype_add_share_file:
        printf("updating: add share\n");
        msgu_share_file(&serv->shares, &delta->update.share_file.share_name);
        break;
    }
    return 1;
}


int msg_server_notify(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    // send notification to any update listeners
    return 1;
}


int msg_server_reply(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    union msgu_any_update update;
    switch (delta->update_type) {
    case msgtype_list_shares:
        printf("reply: list shares\n");
        msgs_node_list_from_path(&update.node_list.nodes, ".");
        msgu_channel_update_send(&delta->source->ch, msgtype_return_node_list, &update);
        msgu_channel_write(&delta->source->ch);
        break;
    }
    return 1;
}

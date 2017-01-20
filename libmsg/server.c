#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *serv = p;
    struct msgs_socket newsocket;

    // which acceptor was updated?
    if (e->id == serv->local_acc_id) {
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
    msg_server_connection_notify(serv, e->id);
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event    = msg_server_connect_event,
    .recv_event       = msg_server_recv_event,
};


int msg_connection_init_handle(struct msg_connection *conn, const struct msgu_string *share_name) {
    uint32_t new_handle = conn->next_handle++;
    struct msgu_string name;
    msgu_string_copy(&name, share_name);
    printf("opening %s as %d\n", share_name->buf, new_handle);
    msgu_map_insert(&conn->handles, &new_handle, &name);
    return new_handle;
}


int msg_connection_read_handle(struct msg_connection *conn, int node_handle) {
    struct msgu_string *name = msgu_map_get(&conn->handles, &node_handle);
    printf("read handle: %d, %s\n", node_handle, name->buf);


}



struct msg_host *msg_server_self(struct msg_server *s) {
    return &s->hosts.ptr[0];
}


int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket) {
    struct msg_connection conn;
    conn.socket = *socket;
    conn.new_events = 0;
    conn.next_handle = 0;
    msgs_mutex_init(&conn.read_mutex);
    msgs_mutex_init(&conn.write_mutex);
    msgu_channel_init(&conn.ch, (msgu_stream_id_t) socket->fd, &msgs_socket_fn);
    msgu_map_init(&conn.handles, msgu_uint32_hash, msgu_uint32_cmp, sizeof(uint32_t), sizeof(struct msgu_string));
    msgu_map_alloc(&conn.handles, 16);


    // lock and modify server state
    msgs_mutex_lock(&s->conn_mutex);
    int id = msgu_add_recv_handler(&s->emap);
    msgu_map_insert(&s->connections, &id, &conn);
    msgs_poll_socket(&s->tb, socket, id);
    msgs_mutex_unlock(&s->conn_mutex);
    return id;
}


int msg_server_close_connection(struct msg_server *s, int id) {
    struct msg_connection *conn = msg_server_connection(s, id);
    msgs_mutex_lock(&s->conn_mutex);
    if (conn) {
        msgs_close_socket(&conn->socket);
        msgu_channel_free(&conn->ch);
    }
    msgu_map_erase(&s->connections, &id);
    msgs_mutex_unlock(&s->conn_mutex);
    return 1;
}


struct msg_connection *msg_server_connection(struct msg_server *s, int id) {
    return msgu_map_get(&s->connections, &id);
}


int msg_server_connection_notify(struct msg_server *serv, int id) {
    struct msg_connection *conn;

    // make sure every path unlocks this
    msgs_mutex_lock(&serv->conn_mutex);
    conn = msg_server_connection(serv, id);
    if (conn) {

        // TODO if not locked, the message may go unread
        // depending on the state of currently reading thread
        conn->new_events = 1;

        if (msgs_mutex_try_lock(&conn->read_mutex)) {
            msgs_mutex_unlock(&serv->conn_mutex);
            int result = msg_server_connection_poll(serv, id, conn);
            msgs_mutex_unlock(&conn->read_mutex);
            if (result == -1) {
                // socket was closed
                printf("connection %d: closed\n", id);
                msg_server_close_connection(serv, id);
            }
        }
        else {
            msgs_mutex_unlock(&serv->conn_mutex);
        }
    }
    else {
        printf("connection %d: not found\n", id);
        msgs_mutex_unlock(&serv->conn_mutex);
    }
}


int msg_server_connection_poll(struct msg_server *serv, int id, struct msg_connection *conn) {
    struct msg_delta delta;
    int read_delta;

    delta.source_id = id;
    delta.source    = conn;

    // read available data from socket
    printf("read channel %d\n", id);
    do {
        read_delta = msgu_channel_try_read(&conn->ch, &delta.update_type, &delta.update);
        if (msgu_channel_is_closed(&conn->ch)) {
            return -1;
        }

        // apply update using current thread
        if (read_delta) {
            msgu_update_print(delta.update_type, &delta.update);
            msg_server_apply(serv, &delta);
            msgu_update_free(delta.update_type, &delta.update);
            msg_server_print_state(serv);
        }
    }
    while(read_delta);
    return 0;
}


void msg_server_print_state(struct msg_server *serv) {
    printf("server state:\n");
    msgu_share_debug(&serv->shares);
}


void msg_server_init(struct msg_server *s, const char *sockpath) {
    msgu_event_map_init(&s->emap, &msg_server_handlers, s);
    msgs_mutex_init(&s->conn_mutex);
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
    s->local_acc_id = msgu_add_connect_handler(&s->emap);
    msgs_poll_acceptor(&s->tb, &s->local_acc, s->local_acc_id);


    // create a remote acceptor
    struct msgu_address raddr;
    ep_listen_remote(&raddr, 2204);
    msgs_open_acceptor(&s->remote_acc, &raddr);
    s->remote_acc_id = msgu_add_connect_handler(&s->emap);
    msgs_poll_acceptor(&s->tb, &s->remote_acc, s->remote_acc_id);
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
    status.error = 0;
    status.new_handle = 0;
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
        break;
    case msgtype_init_remote:
        break;
    case msgtype_add_share_file:
        msgu_share_file(&serv->shares, &delta->update.share_file.share_name);
        break;
    case msgtype_file_open:
        status->new_handle = msg_connection_init_handle(delta->source, &delta->update.share_file.share_name);
        break;
    case msgtype_file_stream_read:
        msg_connection_read_handle(delta->source, delta->update.node_read.node_handle);
        break;
    }
    return 1;
}


int msg_server_notify(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    // send notification to any update listeners
    return 1;
}


int msg_server_reply(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    struct msgu_channel *out = &delta->source->ch;
    int have_update = 0;
    int update_type;
    union msgu_any_update update;


    switch (delta->update_type) {
    case msgtype_list_shares:
        have_update = 1;
        update_type = msgtype_return_share_list;
        msgs_node_list_of_shares(&serv->shares, &update.node_list.nodes);
        break;
    case msgtype_file_open:
        have_update = 1;
        update_type = msgtype_return_node_handle;
        update.node_handle.node_handle = status->new_handle;
        break;
    }


    if (have_update) {
        printf("reply:\n");
        msgu_update_print(update_type, &update);
        msgu_channel_update_send(out, update_type, &update);
        msgu_channel_write(out);
    }
    return 1;
}

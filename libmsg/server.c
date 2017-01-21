#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *serv = p;
    struct msgs_socket newsocket;
    char sockname [128];

    // which acceptor was updated?
    if (e->id == serv->local_acc_id) {
        while (msgs_accept_socket(&serv->local_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (local)\n", sockname, e->id);
            msg_server_init_connection(serv, &newsocket);
        }
    }
    else if (e->id == serv->remote_acc_id) {
        while (msgs_accept_socket(&serv->remote_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (remote)\n", sockname, e->id);
            msg_server_init_connection(serv, &newsocket);
        }
    }
    else {
        printf("[server] accept: %d (unknown)\n", e->id);
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


int msg_server_message_recv(struct msg_connection *conn, struct msgu_message *msg, void *serv) {
    return msg_server_recv(serv, conn, msg);
}


struct msg_host *msg_server_self(struct msg_server *s) {
    return &s->hosts.ptr[0];
}


int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket) {
    struct msg_connection conn;
    msg_connection_init(&conn, socket, msg_server_message_recv, s);
    int id = msgu_add_recv_handler(&s->emap);

    // lock and modify server state
    msgs_mutex_lock(&s->conn_mutex);
    msgu_map_insert(&s->connections, &id, &conn);
    msgs_mutex_unlock(&s->conn_mutex);

    // start polling after adding to connection map
    msgs_poll_socket(&s->tb, socket, id);
    return id;
}


int msg_server_close_connection(struct msg_server *s, int id) {
    struct msg_connection *conn = msg_server_connection(s, id);
    msgs_mutex_lock(&s->conn_mutex);
    if (conn) {
        msg_connection_close(conn);
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
        msg_connection_notify(conn);

        // lock read mutex first
        if (msgs_mutex_try_lock(&conn->read_mutex)) {
            msgs_mutex_unlock(&serv->conn_mutex);
            int result = msg_connection_poll(conn);
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


void msg_server_print_state(struct msg_server *serv) {
    printf("[server state] ");
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


int msg_server_recv(struct msg_server *serv, struct msg_connection *conn, struct msgu_message *msg) {
    msg_connection_log(conn, msg, "recv");
    msg_server_apply(serv, conn, msg);
    msgu_message_free(msg);
    msg_server_print_state(serv);
    return 0;
}


int msg_server_apply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg) {
    struct msg_status status;
    status.error = 0;
    status.new_handle = 0;
    if (msg_server_validate(serv, conn, msg)) {
        if (msg_server_modify(serv, conn, msg, &status)) {
            msg_server_notify(serv, conn, msg, &status);
            msg_server_reply(serv, conn, msg, &status);
        }
        else {
            return -1;
        }
    }
    return 0;
}


int msg_server_validate(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg) {
    // check update is valid
    return 1;
}


int msg_server_modify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, struct msg_status *status) {
    struct msgu_file_event fevent;
    char path [256];
    int i;

    // lock mutex and apply state changes
    switch (msg->event_type) {
    case msgtype_init_local:
        break;
    case msgtype_init_remote:
        break;
    case msgtype_add_share_file:
        msgu_share_file(&serv->shares, &msg->buf.data.share_file.share_name);
        break;
    case msgtype_file_open:
        status->new_handle = msg_connection_init_handle(conn, &msg->buf.data.share_file.share_name);
        break;
    case msgtype_file_stream_read:
        msg_connection_read_handle(conn, msg->buf.data.node_read.node_handle);
        break;
    }
    return 1;
}


int msg_server_notify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status) {
    // send notification to any update listeners
    return 1;
}


int msg_server_reply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status) {
    int have_update = 0;
    int event_type;
    int data_type;
    union msgu_any_msg data;


    switch (msg->event_type) {
    case msgtype_list_shares:
        have_update = 1;
        event_type = msgtype_return_share_list;
        data_type = msgdata_node_list;
        msgs_node_list_of_shares(&serv->shares, &data.node_list.nodes);
        break;
    case msgtype_file_open:
        have_update = 1;
        event_type = msgtype_return_node_handle;
        data_type = msgdata_node_handle;
        data.node_handle.node_handle = status->new_handle;
        break;
    }

    if (have_update) {
        msg_connection_send_message(conn, event_type, data_type, &data);
    }
    return 1;
}

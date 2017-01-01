#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *serv = p;
    struct msgs_socket newsocket;

    // which acceptor was updated?
    while (msgs_accept_socket(&serv->local_acc, &newsocket)) {
        printf("server connect event id %d\n", e->id);
        msg_server_init_connection(serv, &newsocket);
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
        while ((delta.update_type = msgu_channel_read(&conn->ch, &delta.update)) > 0) {
            msg_server_apply(serv, &delta);
        }
        if (delta.update_type < 0) {
            // socket was closed
        }
    }
    else {
        printf("connection %d not found\n", e->id);
    }
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event    = msg_server_connect_event,
    .recv_event       = msg_server_recv_event,
};


struct msg_host *msg_server_self(struct msg_server *s) {
    return &s->hosts.ptr[0];
}


void msg_server_printsub(struct msg_server *s) {
    printf("%lu sub keys\n", s->node_to_sub.keys.elem_count);
    printf("%lu sub values\n", s->node_to_sub.value_count);

    // find all the keys
    for (int i = 0; i < s->node_to_sub.keys.elem_count; ++i) {
        struct msgu_subarray *sa = msgu_multimap_get_index(&s->node_to_sub, i);
        printf("key %d (%d to %d)\n", i, sa->begin, sa->end);
    }
}


int msg_node_of_host(struct msg_server *s, int epid) {
    int *nodelist = msgu_multimap_get(&s->host_to_tree, epid, 0);
    if (nodelist) {
        return nodelist[0];
    }
    else {
        return 0;
    }
}


void msg_server_add_share(struct msg_server *serv, struct msgu_stream *s) {
    struct msgu_file_event fe;
    char path [256];
    msgu_stream_read(s, path, sizeof(path));
    printf("share %s\n", path);
    int i = msgu_add_file(&serv->emap, &fe);
    msg_share_file(&serv->shares);
}


void msg_server_add_client(struct msg_server *s, int epid, int nodeid) {
    printf("connect epid %d => node %d\n", epid, nodeid);
    int index = msgu_multimap_insert(&s->host_to_tree, epid, 1);
    int *nodelist = msgu_multimap_get(&s->host_to_tree, epid, index);
    nodelist[0] = nodeid;
}


void msg_server_rm_client(struct msg_server *s, int i) {

}


int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket) {
    struct msg_connection conn;
    conn.socket = *socket;
    msgu_stream_init(&conn.ch.stream, socket->fd, &msgs_socket_fn);
    msgu_stat_reset(&conn.ch.stat);
    int id = msgs_poll_socket(&s->tb, socket);
    msgu_map_insert(&s->connections, &id, &conn);
    return id;
}


struct msg_connection *msg_server_connection(struct msg_server *s, int id) {
    return msgu_map_get(&s->connections, &id);
}


void msg_server_init(struct msg_server *s, const char *sockpath) {
    msgu_event_map_init(&s->emap, &msg_server_handlers, s);
    msgu_map_init(&s->connections, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct msg_connection));
    msgu_map_alloc(&s->connections, 1024);
    msgu_multimap_init(&s->host_to_tree, sizeof(int), 1024);
    msgu_multimap_init(&s->node_to_sub, sizeof(struct msgu_channel), 1024);
    msgs_table_init(&s->tb, &s->emap);

    // find own address and hostname
    struct ep_host host;
    ep_host_init_self(&host);
    msg_host_list_init(&s->hosts);
    msg_host_list_alloc(&s->hosts, 32);
    msg_host_list_add(&s->hosts, host.addr, host.hostname);
    msg_share_set_init(&s->shares);

    // create a local acceptor
    struct msgu_address local_addr;
    ep_unlink("msgd-ipc");
    ep_local(&local_addr, "msgd-ipc");
    msgs_open_acceptor(&s->local_acc, &local_addr);
    msgs_poll_acceptor(&s->tb, &s->local_acc);

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
    msg_req_peer_init(&conn->ch.stream, &serv->hosts.ptr[0]);
    return cid;
}


void msg_server_run(struct msg_server *serv) {

    // waits until threads complete
    msgs_table_start(&serv->tb, 4);
    msgs_table_free(&serv->tb);
}


int msg_server_apply(struct msg_server *serv, const struct msg_delta *delta) {
    struct msg_status status;
    printf("applying msg\n");
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
    // lock mutex and apply state changes
    printf("apply %d\n", delta->update_type);

    // print new state
    msg_host_list_debug(&serv->hosts);
    msg_server_printsub(serv);
    return 1;
}


int msg_server_notify(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    // send notification to any update listeners
    return 1;
}


int msg_server_reply(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status) {
    return 1;
}

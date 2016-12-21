#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *s = p;

    printf("server connect event\n");
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event = msg_server_connect_event,
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

        // print values
        for (int i = sa->begin; i < sa->end; ++i) {
            size_t addr = sizeof(struct msgu_channel) * i;
            struct msgu_channel *sub = (struct msgu_channel *) &s->node_to_sub.values[addr];
            printf("\tsub %d => %d\n", sub->subid, sub->epid);
        }
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


void msg_server_add_share(struct msg_server *serv, struct msgu_buffer *buf) {
    struct msgu_file_event fe;
    char path [256];
    ep_buffer_peek(buf, path, 0, buf->size);
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
    struct msg_connection ch;
    ch.socket = *socket;
    msgu_buffer_init(&ch.read_buf, malloc(1024), 1024);
    msgu_buffer_init(&ch.write_buf, malloc(1024), 1024);
    int id = msgs_poll_socket(&s->tb, socket);
    msgu_map_insert(&s->connections, &id, &ch);
    return id;
}


struct msg_connection *msg_server_connection(struct msg_server *s, int id) {
    return msgu_map_get(&s->connections, &id);
}


void msg_server_subscribe(struct msg_server *s, int epid, struct msgu_buffer *buf) {
    struct msg_subscribe subs;
    ep_buffer_peek(buf, (char *) &subs, 0, sizeof(subs));
    printf("subscribe updates on node id %d to epid %d\n", subs.nodeid, epid);
    int index = msgu_multimap_insert(&s->node_to_sub, subs.nodeid, 1);
    struct msgu_channel *sub = msgu_multimap_get(&s->node_to_sub, subs.nodeid, index);
    if (sub) {
        sub->epid = epid;
        sub->subid = subs.subid;
    }
    else {
        printf("id %d not found\n", subs.nodeid);
    }
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

    struct msgs_acceptor local_acc;
    msgs_open_acceptor(&local_acc, &local_addr);
    msgs_poll_acceptor(&s->tb, &local_acc);

    // create a remote acceptor
    struct msgu_address raddr;
    struct msgs_acceptor acc;
    ep_listen_remote(&raddr, 2204);
    msgs_open_acceptor(&acc, &raddr);
    msgs_poll_acceptor(&s->tb, &acc);
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
    msg_req_peer_init(&conn->write_buf, &serv->hosts.ptr[0]);
    return cid;
}


void msg_server_run(struct msg_server *serv) {

    // waits until threads complete
    msgs_table_start(&serv->tb, 4);
    msgs_table_free(&serv->tb);
}


void msg_server_apply(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out) {
    struct msg_host *self = msg_server_self(serv);

    if (m->head.share_id < 0) {

    }
    else {

    }
}


void msg_server_apply_local(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out) {
    struct msg_host *self = msg_server_self(serv);

    // recv from local processes

    if (m->head.share_id < 0) {
        // requests for shared memory, or registering new processes and files
        printf("recv type %d (%d bytes)\n", m->head.type, m->head.size);
        switch (m->head.type) {
        case msg_type_peer_init:
            msg_host_list_merge(&serv->hosts, m->body, 0);
            msg_send_host_list(&serv->hosts, out);
            break;
        case msg_type_peer_update:
            printf("TODO: peer update\n");
            break;
        case msg_type_peer_one:
        case msg_type_peer_all:
            msg_merge_peers(&serv->hosts, m->body, 0);
            break;
        case msg_type_share_proc:
        case msg_type_share_file:
            msg_server_add_share(serv, m->body);
            msg_send_host(self, out);
            break;
        }
    }
    else {
        // request gets passed to existing shares
        msg_server_apply_share(serv, srcid, m, out);
    }
}


void msg_server_apply_remote(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out) {
    if (m->head.share_id < 0) {

    }
    else {
        // request gets passed to existing shares
        msg_server_apply_share(serv, srcid, m, out);
    }

}


void msg_server_apply_share(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out) {
    printf("recv update for share %d\n", m->head.share_id);
     // TODO shares
}



void msg_server_recv(struct msg_server *serv, int src_epid, struct msgu_buffer *buf) {
    printf("recv from: %d\n", src_epid);
    printf("initial bytes: %lu\n", buf->size);

    // TODO find socket
    struct msgs_socket *out = NULL;
    msg_server_reply(serv, src_epid, buf, out);
    msg_host_list_debug(&serv->hosts);
    printf("server config:\n");
    msg_server_printsub(serv);
    printf("remaining bytes: %lu\n\n", buf->size);
}


void msg_server_reply(struct msg_server *serv, int src_epid, struct msgu_buffer *in, struct msgs_socket *out) {
    struct msg_message msg;
    struct msg_connection *conn = msg_server_connection(serv, src_epid);
    while(msg_poll_message(in, &msg)) {
        msg_server_apply(serv, src_epid, &msg, &conn->write_buf);
        ep_buffer_release(msg.body, msg.head.size);
    }
}

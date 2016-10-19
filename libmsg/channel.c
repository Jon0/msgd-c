#include <stdio.h>
#include <string.h>

#include "channel.h"
#include "protocol.h"


int msg_channel_id(void *p) {
    struct msg_channel *c = (struct msg_channel *) p;
    return c->epid;
}


struct ep_tree *msg_server_self(struct msg_server *s) {
    return &s->hosts[0].shared_tree;
}


int msg_server_init_host(struct msg_server *s) {
    // find own address and hostname
    s->hosts = malloc(sizeof(struct msg_host) * 32);
    s->host_count = 0;
    msg_server_add_host(s, "127.0.0.1", "testhost");
}


int msg_server_add_host(struct msg_server *s, const char *addr, const char *name) {
    int hostid = s->host_count++;
    struct msg_host *h = &s->hosts[hostid];
    h->active_id = 0;
    strcpy(h->addr, addr);
    strcpy(h->hostname, name);
    msg_tree_init(&h->shared_tree);
    return hostid;
}


void msg_server_printsub(struct msg_server *s) {
    printf("%d sub keys\n", s->node_to_sub.keys.elem_count);
    printf("%d sub values\n", s->node_to_sub.value_count);

    // find all the keys
    for (int i = 0; i < s->node_to_sub.keys.elem_count; ++i) {
        struct ep_subarray *sa = ep_multimap_get_index(&s->node_to_sub, i);
        printf("key %d (%d to %d)\n", sa->key, sa->begin, sa->end);

        // print values
        for (int i = sa->begin; i < sa->end; ++i) {
            size_t addr = sizeof(struct msg_subscriber) * i;
            struct msg_subscriber *sub = (struct msg_subscriber *) &s->node_to_sub.values[addr];
            printf("\tsub %d => %d\n", sub->subid, sub->epid);
        }
    }
}


int msg_node_of_host(struct msg_server *s, int epid) {
    printf("TODO: node_of_host\n");
    return 0;
}


void msg_server_add_client(struct msg_server *s, int epid, int nodeid) {
    printf("connect epid %d => %d\n", epid, nodeid);
    ep_multimap_insert(&s->host_to_tree, epid, 1);
}


void msg_server_rm_client(struct msg_server *s, int i) {

}


void msg_server_init_channel(struct msg_server *s, int epid) {
    struct msg_channel ch;
    ch.epid = epid;
    ch.type = 0;
    ch.subs = 0;
    ep_map_insert(&s->socket_type, &ch);
}


void msg_server_subscribe(struct msg_server *s, int sendnode, int epid, int subid) {
    printf("subscribe updates on node id %d to epid %d\n", sendnode, epid);
    int index = ep_multimap_insert(&s->node_to_sub, sendnode, 1);
    struct msg_subscriber *sub = ep_multimap_get(&s->node_to_sub, sendnode, index);
    if (sub) {
        sub->epid = epid;
        sub->subid = subid;
    }
    else {
        printf("id %d not found\n", sendnode);
    }
}


void msg_server_init(struct msg_server *s, const char *sockpath) {

    // alloc structures
    ep_map_alloc(&s->socket_type, msg_channel_id, sizeof(struct msg_channel), 1024);
    ep_multimap_init(&s->host_to_tree, sizeof(int), 1024);
    ep_multimap_init(&s->node_to_sub, sizeof(struct msg_subscriber), 1024);
    ep_table_init(&s->tb, 256);
    msg_server_init_host(s);

    // start threads
    ep_thread_pool_create(&s->pool, &s->tb, 4, 0);

    // create an acceptor
    struct ep_acceptor acc;
    ep_listen_remote(&acc.addr, 2204);
    ep_init_acceptor(&acc, msg_server_accept, s);
    ep_add_acceptor(&s->tb, &acc);
}


int msg_server_connect(struct msg_server *serv, const char *addr) {
    struct ep_channel ch;
    ep_connect_remote(&ch.addr, addr, 2204);
    int err = ep_init_channel(&ch);
    if (err == -1) {
        return err;
    }
    int remote = ep_add_channel(&serv->tb, &ch);

    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_server_handler, serv);
    int hid = ep_add_handler(&serv->tb, &hdl);
    ep_table_route(&serv->tb, remote, hid);
    msg_server_init_channel(serv, remote);

    // request table of known addresses
    msg_req_peer_init(&ch.write_buf, &serv->hosts[0]);
    ep_channel_flush(&ch);
    return remote;
}


void msg_server_run(struct msg_server *serv) {

    // main loop
    ep_queue_from_table(&serv->pool.queue);

    // wait until threads complete
    ep_thread_pool_join(&serv->pool);
    ep_table_free(&serv->tb);
}


int msg_server_poll_message(struct ep_buffer *in, struct msg_message *out) {

    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t read_header;
    size_t read_body;
    if (in->size >= hs) {
        read_header = ep_buffer_peek(in, (char *) &out->head, 0, hs);
        if (read_header == hs && in->size >= hs + out->head.size) {
            ep_buffer_release(in, hs);
            out->body = in;
            return 1;
        }
    }
    return 0;
}


void msg_server_apply(struct msg_server *serv, int srcid, struct msg_message *m, struct ep_buffer *out) {
    struct ep_tree *self_tree = msg_server_self(serv);
    int newid;
    int subints [2];

    // apply actions based on message type
    printf("recv type %d (%d bytes)\n", m->head.id, m->head.size);
    switch (m->head.id) {
    case msg_type_peer_init:
        printf("recv host\n");
        newid = serv->host_count++;
        msg_host_recv(m->body, &serv->hosts[newid]);
        printf("return all hosts\n");
        msg_send_peers(out, serv->hosts, serv->host_count);
        break;
    case msg_type_peer_update:
        printf("TODO: peer update\n");
        break;
    case msg_type_peer_one:
    case msg_type_peer_all:
        msg_merge_peers(m->body, serv->hosts, serv->host_count, 32);
        break;
    case msg_type_proc:
        newid = msg_tree_add_proc(self_tree, m->body, m->head.size);
        msg_server_add_client(serv, srcid, newid);
        msg_tree_send(self_tree, out);
        break;
    case msg_type_publ:
        newid = msg_node_of_host(serv, srcid);
        msg_tree_subnode(self_tree, m->body, m->head.size, newid);
        msg_tree_send(self_tree, out);
        break;
    case msg_type_subs:
        ep_buffer_peek(m->body, (char *) &subints, 0, sizeof(subints));
        msg_server_subscribe(serv, subints[0], srcid, subints[1]);
        msg_tree_send(self_tree, out);
    case msg_type_avail:
        msg_tree_send(self_tree, out);
        break;
    }
    printf("message applied\n");
}


void msg_server_recv(struct msg_server *serv, int src_epid, struct ep_buffer *buf) {
    printf("recv from: %d\n", src_epid);
    printf("initial bytes: %d\n", buf->size);

    struct ep_table_entry *e = ep_map_get(&serv->tb.entries, src_epid);
    if (e) {
        msg_server_reply(serv, src_epid, buf, &e->data.ch);
        msg_server_print_debug(serv);
    }
    printf("remaining bytes: %d\n\n", buf->size);
}


void msg_server_reply(struct msg_server *serv, int src_epid, struct ep_buffer *in, struct ep_channel *out) {
    struct msg_message msg;
    while(msg_server_poll_message(in, &msg)) {
        msg_server_apply(serv, src_epid, &msg, &out->write_buf);
        ep_buffer_release(msg.body, msg.head.size);
    }
    ep_channel_flush(out);
}


void msg_server_print_debug(struct msg_server *serv) {
    printf("\n=== server state ===\n");
    for (int i = 0; i < serv->host_count; ++i) {
        struct msg_host *host = &serv->hosts[i];
        printf("[%s] Host id %d: %s\n", host->addr, i, host->hostname);
        ep_tree_print(&host->shared_tree);
        msg_tree_elems(&host->shared_tree);
    }
    printf("current config:\n");
    msg_server_printsub(serv);
}


void msg_server_accept(struct ep_table *t, int epid, void *serv) {
    printf("new connection id %d\n", epid);
    ep_table_print_id(t, epid);
    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_server_handler, serv);
    int index = ep_multimap_insert(&t->chanout, epid, 1);
    int *out = ep_multimap_get(&t->chanout, epid, index);
    *out = ep_add_handler(t, &hdl);
    msg_server_init_channel(serv, epid);
}


void msg_server_handler(int ex, struct ep_event_view *ev) {
    if (ex) {
        printf("exception %d\n", ex);
    }
    else {
        struct msg_server *serv = (struct msg_server *) ev->self->data;
        msg_server_recv(serv, ev->src, &ev->self->buf);
    }
}

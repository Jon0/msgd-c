#include <stdio.h>
#include <string.h>

#include "channel.h"
#include "protocol.h"


int msg_channel_id(void *p) {
    struct msg_channel *c = (struct msg_channel *) p;
    return c->epid;
}


struct msg_host *msg_server_self(struct msg_server *s) {
    return &s->hosts[0];
}


int msg_server_init_host(struct msg_server *s) {
    struct ep_host host;
    ep_host_init_self(&host);


    // find own address and hostname
    size_t host_max = 32;
    s->hosts = malloc(sizeof(struct msg_host) * host_max);
    s->host_count = 1;
    msg_host_init(&s->hosts[0], host.addr, host.hostname);
    for (int i = 0; i < host_max; ++i) {
        msg_tree_init(&s->hosts[i].shared_tree);
    }
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
    int *nodelist = ep_multimap_get(&s->host_to_tree, epid, 0);
    if (nodelist) {
        return nodelist[0];
    }
    else {
        return 0;
    }
}


void msg_server_add_client(struct msg_server *s, int epid, int nodeid) {
    printf("connect epid %d => node %d\n", epid, nodeid);
    int index = ep_multimap_insert(&s->host_to_tree, epid, 1);
    int *nodelist = ep_multimap_get(&s->host_to_tree, epid, index);
    nodelist[0] = nodeid;
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


void msg_server_subscribe(struct msg_server *s, int epid, struct ep_buffer *buf) {
    struct msg_subscribe subs;
    ep_buffer_peek(buf, (char *) &subs, 0, sizeof(subs));
    printf("subscribe updates on node id %d to epid %d\n", subs.nodeid, epid);
    int index = ep_multimap_insert(&s->node_to_sub, subs.nodeid, 1);
    struct msg_subscriber *sub = ep_multimap_get(&s->node_to_sub, subs.nodeid, index);
    if (sub) {
        sub->epid = epid;
        sub->subid = subs.subid;
    }
    else {
        printf("id %d not found\n", subs.nodeid);
    }
}


void msg_server_read_data(struct msg_server *serv, struct ep_buffer *buf) {
    struct msg_node_update ud;
    ep_buffer_peek(buf, (char *) &ud, 0, sizeof(int) * 2);

    //
    ud.len = 0;

    // pass to all subscribed processes
    // and all peers with at least one subscriber
    struct ep_subarray *sa = ep_multimap_get_key(&serv->node_to_sub, ud.nodeid);
    if (sa) {
        printf("recv data (node %d, handle %d, range %d to %d)", ud.nodeid, ud.hdlid, sa->begin, sa->end);
        for (int i = sa->begin; i < sa->end; ++i) {
            struct msg_subscriber *sub = ep_multimap_get_value(&serv->node_to_sub, i);
            msg_server_reply_data(serv, sub->epid, &ud);

        }
    }
    else {
        printf("node %d doesnt exist\n", ud.nodeid);
    }
}


void msg_server_reply_data(struct msg_server *serv, int epid, struct msg_node_update *u) {
    struct ep_table_entry *e = ep_map_get(&serv->tb.entries, epid);
    struct ep_channel *ch = &e->data.ch;
    msg_send_block(&ch->write_buf, u->nodeid, u->hdlid, u->buf, u->len);
    printf("[%d] write %d bytes\n", epid, ch->write_buf.size);
    ep_channel_flush(ch);
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


void msg_server_apply(struct msg_server *serv, int srcid, struct msg_message *m, struct ep_buffer *out) {
    struct msg_host *self = msg_server_self(serv);
    int newid;

    // apply actions based on message type
    printf("recv type %d (%d bytes)\n", m->head.id, m->head.size);
    switch (m->head.id) {
    case msg_type_peer_init:
        newid = serv->host_count++;
        msg_host_merge(m->body, 0, serv->hosts, &serv->host_count);
        msg_send_peers(out, serv->hosts, serv->host_count);
        break;
    case msg_type_peer_update:
        printf("TODO: peer update\n");
        break;
    case msg_type_peer_one:
    case msg_type_peer_all:
        msg_merge_peers(m->body, serv->hosts, &serv->host_count, 32);
        printf("host count is now %d\n", serv->host_count);
        break;
    case msg_type_proc_init:
        newid = msg_tree_add_proc(&self->shared_tree, m->body, m->head.size);
        msg_server_add_client(serv, srcid, newid);
        msg_send_self(out, self);
        break;
    case msg_type_publ:
        newid = msg_node_of_host(serv, srcid);
        msg_tree_subnode(&self->shared_tree, m->body, m->head.size, newid);
        msg_send_self(out, self);
        break;
    case msg_type_subs:
        msg_server_subscribe(serv, srcid, m->body);
        msg_send_self(out, self);
    case msg_type_avail:
        msg_send_self(out, self);
        break;
    case msg_type_data:
        msg_server_read_data(serv, m->body);
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
    while(msg_poll_message(in, &msg)) {
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

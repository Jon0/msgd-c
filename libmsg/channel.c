#include <stdio.h>

#include "channel.h"
#include "protocol.h"


int msg_channel_id(void *p) {
    struct msg_channel *c = (struct msg_channel *) p;
    return c->epid;
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
    msg_tree_init(&s->shared_tree);
    msg_tree_set_name(&s->shared_tree, "testhost");
    ep_table_init(&s->tb, 256);

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

    // TODO request table of known addresses
}


void msg_server_run(struct msg_server *serv) {

    // main loop
    ep_queue_from_table(&serv->pool.queue);

    // wait until threads complete
    ep_thread_pool_join(&serv->pool);
    ep_table_free(&serv->tb);
}


void msg_server_recv(struct msg_server *serv, int epid) {
    printf("TODO recv\n");
}


void msg_server_peer_reply(struct msg_server *serv) {

}


void msg_server_client_reply(struct msg_server *serv, int src_epid) {
    struct msg_request req;

    // TODO find buffer from epid
    struct ep_buffer *buf;
    //req.buf = buf;
    //req.src = &ev->src;
    printf("recv from: %d\n", src_epid);
    printf("initial bytes: %d\n", buf->size);
    msg_poll_apply(serv, &req);
    msg_server_print_debug(serv);
    printf("remaining bytes: %d\n\n", buf->size);
}


void msg_server_print_debug(struct msg_server *serv) {
    printf("\n=== server state ===\n");
    ep_tree_print(&serv->shared_tree);
    msg_tree_elems(&serv->shared_tree);
    msg_server_printsub(serv);
}


void msg_server_accept(struct ep_table *t, int epid, void *in) {
    printf("new connection id %d\n", epid);
    ep_table_print_id(t, epid);
    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_server_handler, in);
    int index = ep_multimap_insert(&t->chanout, epid, 1);
    int *out = ep_multimap_get(&t->chanout, epid, index);
    *out = ep_add_handler(t, &hdl);
}


void msg_server_handler(int ex, struct ep_event_view *ev) {
    if (ex) {
        printf("exception %d\n", ex);
    }
    else {
        struct msg_server *serv = (struct msg_server *) ev->self->data;
        msg_server_recv(serv, ev->epid);
    }
}

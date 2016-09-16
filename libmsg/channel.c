#include <stdio.h>

#include "channel.h"
#include "protocol.h"


int msg_node_of_host(struct msg_server *s, int epid) {
    printf("TODO: node_of_host\n");
    return 0;
}

void msg_server_connect(struct msg_server *s, int epid, int nodeid) {
    printf("connect epid %d => %d\n", epid, nodeid);
    ep_multimap_insert(&s->host_to_tree, epid, 1);
}


void msg_server_disconnect(struct msg_server *s, int i) {

}


void msg_server_subscribe(struct msg_server *s, int key, int epid) {
    printf("subscribe node id %d\n", key);
    int index = ep_multimap_insert(&s->node_to_sub, key, 1);
    struct msg_subscriber *s = ep_multimap_get(&s->node_to_sub, key, index);
    s->epid = epid;

    // TODO recieve subid from client
    s->subid = 0;
}


int msg_server_accept(struct ep_table *t, void *in, int *out) {
    printf("accept\n");
    struct ep_handler hdl;
    ep_handler_init(&hdl, 4096, msg_server_recv, in);
    *out = ep_add_handler(t, &hdl);
    return 1;
}


void msg_server_recv(int ex, struct ep_event_view *ev) {
    struct msg_server *serv = (struct msg_server *) ev->self->data;
    struct ep_tree *tree = &serv->tree;
    struct msg_request req;
    req.buf = &ev->self->buf;
    req.src = &ev->src;
    printf("recv from: %d\n", ev->epid);
    printf("initial bytes: %d\n", ev->self->buf.size);
    msg_poll_apply(serv, &req);

    // print tree state
    ep_tree_print(tree);
    msg_tree_elems(tree);
    printf("remaining bytes: %d\n", ev->self->buf.size);
}


void msg_server_run(struct msg_server *s, const char *sockpath) {

    // alloc structures
    ep_multimap_init(&s->host_to_tree, sizeof(int), 1024);
    ep_multimap_init(&s->node_to_sub, sizeof(struct msg_subscriber), 1024);
    msg_tree_init(&s->tree);
    msg_tree_set_name(&s->tree, "testhost");
    ep_tree_print(&s->tree);
    ep_table_init(&s->tb, 256);

    // start threads
    ep_thread_pool_create(&s->pool, &s->tb, 4, 0);

    // create an acceptor
    struct ep_acceptor acc;
    ep_listen_remote(&acc.addr, 2204);
    ep_init_acceptor(&acc, msg_server_accept, s);
    ep_add_acceptor(&s->tb, &acc);

    // main loop
    ep_queue_from_table(&s->pool.queue);

    // wait until threads complete
    ep_thread_pool_join(&s->pool);
    ep_table_free(&s->tb);
}

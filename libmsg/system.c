#include "system.h"


void msg_system_init(struct msg_system *s, struct msgu_handlers *hdls, void *arg) {
    msgu_event_map_init(&s->emap, hdls, arg);
    msgs_table_init(&s->tb, &s->emap);
    s->fuse = msgs_fuse_static_start(&s->emap, "fusemount");
}


int msg_system_create_acceptor(struct msg_system *s, struct msgu_address *addr, struct msgs_acceptor *acc) {
    msgs_open_acceptor(acc, addr);
    int id = msgu_add_connect_handler(&s->emap);
    msgs_poll_acceptor(&s->tb, acc, id);
    return id;
}


int msg_system_create_handler(struct msg_system *s) {
    return msgu_add_recv_handler(&s->emap);
}


void msg_system_poll_handler(struct msg_system *s, struct msgs_socket *sk, int id) {
    msgs_poll_socket(&s->tb, sk, id);
}


void msg_mount_notify_node(struct msg_system *s, int exists, const struct msgu_node *nd) {
    s->fuse->reply.exists = exists;
    s->fuse->reply.node = *nd;
    msgs_fuse_notify(s->fuse);
}


void msg_mount_notify_nodes(struct msg_system *s, int exists, const struct msgu_vector *nds) {
    s->fuse->reply.exists = exists;
    s->fuse->reply.nodes = *nds;
    msgs_fuse_notify(s->fuse);
}


void msg_mount_notify_str(struct msg_system *s, const struct msgu_string *str) {
    msgu_string_copy(&s->fuse->reply.data, str);
    msgs_fuse_notify(s->fuse);
}


void msg_mount_notify(struct msg_system *s) {
    msgs_fuse_notify(s->fuse);
}

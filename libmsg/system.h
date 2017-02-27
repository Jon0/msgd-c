#ifndef LIBMSG_SYSTEM_H
#define LIBMSG_SYSTEM_H

#include <libsys/fuse.h>
#include <libsys/table.h>


/*
 * connects to low level systems
 * such as epoll and fuse
 */
struct msg_system {
    struct msgs_event_map emap;
    struct msgs_table     tb;
    struct msgs_fuse_files *fuse;
};


void msg_system_init(struct msg_system *s, struct msgu_handlers *hdls, void *arg);


/*
 * create acceptor and begin polling
 */
int msg_system_create_acceptor(struct msg_system *s, struct msgu_address *addr, struct msgs_acceptor *acc);


/*
 * create new handlers for epoll events
 */
int msg_system_create_handler(struct msg_system *s);
void msg_system_poll_handler(struct msg_system *s, struct msgs_socket *sk, int id);


/*
 * server returns nodes and notifies waiting fuse process
 */
void msg_mount_notify_node(struct msg_system *s, int exists, const struct msgu_node *nd);
void msg_mount_notify_nodes(struct msg_system *s, int exists, const struct msgu_vector *nds);
void msg_mount_notify_str(struct msg_system *s, const struct msgu_string *str);
void msg_mount_notify(struct msg_system *s);


#endif

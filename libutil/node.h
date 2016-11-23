#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include "buffer.h"
#include "tree.h"


/*
 * replace host
 */
struct ep_location {
    int name;
};


/*
 * per node file system functions
 */
enum ep_node_operation {
    ep_op_open,
    ep_op_close,
    ep_op_read,
    ep_op_write,
};


/*
 * nodes are either directories or openable files and sockets
 */
enum msg_node_type {
    msg_node_client,
    msg_node_directory,
    msg_node_socket,
    msg_node_file,
    msg_node_function,
};


/*
 * attributes per node
 * client requires independent or broadcast sockets
 */
struct msg_node_attr {
    int32_t connection_limit;
    int32_t server_mode;
};


/*
 * message from remote node
 * nodeid -1, for replies
 * hdlid -1, for broadcast
 */
struct msg_node_update {
    int nodeid;
    int hdlid;
    char *buf;
    size_t len;
};


/*
 * processes and published nodes
 */
struct msg_node {
    int64_t   mode;
    int64_t   flags;
    char      name [256];
};


/*
 * names a specific recieving process
 */
struct node_dest {
    char proc_name [256];
    void (*update)();
};


/*
 * writable outputs
 * allows wildcard hostname::procname::*
 */
struct node_routes {
    int64_t fd;
    char route_name [256];
};


struct msg_delta_header {
    int32_t size;
    int32_t checksum;
};


/*
 * include in client requests,
 * allowing a delta update to be sent
 */
int32_t msg_tree_hash(struct ep_tree *t);
void msg_tree_init(struct ep_tree *t);
void msg_tree_elems(struct ep_tree *t);

/*
 * add a subnode
 */
struct msg_node *msg_tree_add_node(struct ep_tree *t, struct msg_node *p);
int msg_tree_add_proc(struct ep_tree *t, struct ep_buffer *procname, size_t count);
int msg_tree_subnode(struct ep_tree *t, struct ep_buffer *nodename, size_t count, int id);

#endif

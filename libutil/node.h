#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include "buffer.h"
#include "tree.h"


/*
 * functions on nodes
 */
typedef size_t (*ep_node_read_t)(void *, char *, size_t);
typedef size_t (*ep_node_write_t)(void *, const char *, size_t);


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
enum ep_node_type {
    ep_node_client,
    ep_node_directory,
    ep_node_socket,
    ep_node_file,
    ep_node_link,
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
    enum ep_node_type type;
    int64_t   mode;
    int64_t   flags;
    char      name [256];
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

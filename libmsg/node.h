#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include <libutil/buffer.h>
#include <libutil/queue.h>
#include <libutil/tree.h>


/*
 * processes and published nodes
 */
struct msg_node {
    int64_t   mode;
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
int msg_tree_hash(struct ep_tree *t);
void msg_tree_init(struct ep_tree *t);
void msg_tree_elems(struct ep_tree *t);

/*
 * add a subnode
 */
struct msg_node *msg_tree_add_node(struct ep_tree *t, struct msg_node *p);
void msg_tree_set_name(struct ep_tree *t, const char *hostname);
int msg_tree_add_proc(struct ep_tree *t, struct ep_buffer *procname, size_t count);
int msg_tree_subnode(struct ep_tree *t, struct ep_buffer *nodename, size_t count, int id);

#endif

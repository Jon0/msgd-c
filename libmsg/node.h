#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include <libutil/buffer.h>


/*
 * processes and published nodes
 */
struct msg_tree_node {
    int node_id;
    struct msg_tree_node  *subnodes;
};


/*
 * allocated memory for nodes
 */
struct msg_node_buffer {
    struct msg_tree_node *arr;
    size_t size;
    size_t avail;
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


struct node_base {
    int64_t   id;
    int64_t   mode;
    char      name [256];
};


struct node_src {
    struct node_base   node;
    int64_t            fd;
    struct node_dest  *sinks;
    int64_t            sinks_count;
    char               pipetype [256];
};


struct msg_tree {
    struct msg_node_buffer buf;
    struct msg_tree_node  *root;
    struct node_base   self;
    struct node_base  *subnodes;
    int64_t            size;
};


void msg_node_buffer_init(struct msg_node_buffer *buf);

/*
 * include in client requests,
 * allowing a delta update to be sent
 */
int msg_tree_hash(struct msg_tree *t);

void msg_tree_init(struct msg_tree *t, char *hostname);
void msg_tree_add_proc(struct msg_tree *t, char *procname, size_t count);

void msg_serialise_tree(struct ep_buffer *b, struct msg_tree *n);
void msg_serialise_node(struct ep_buffer *b, struct msg_tree_node *n);


#endif

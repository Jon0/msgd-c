#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include <libutil/buffer.h>
#include <libutil/queue.h>


struct msg_node_attr {
    int node_id;
    char name [64];
};


/*
 * processes and published nodes
 */
struct msg_tree_node {
    struct msg_node_attr   attr;
    struct msg_tree_node  *subnodes;
    size_t                 subnode_count;
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


struct msg_delta_header {
    int32_t size;
    int32_t checksum;
};


void msg_node_buffer_init(struct msg_node_buffer *buf);

/*
 * include in client requests,
 * allowing a delta update to be sent
 */
int msg_tree_hash(struct msg_tree *t);


/*
 * print out all nodes
 */
void msg_tree_print(struct msg_tree *t);



void msg_tree_init(struct msg_tree *t);
void msg_tree_set_name(struct msg_tree *t, const char *hostname);
void msg_tree_add_proc(struct msg_tree *t, const char *procname, size_t count);


/*
 * serialise node tree
 */
void msg_read_tree(struct ep_buffer *b, struct msg_tree *tree);
void msg_read_node(struct ep_buffer *b, struct msg_tree_node *n);
void msg_write_tree(struct ep_buffer *b, struct msg_tree *tree);
void msg_write_node(struct ep_buffer *b, struct msg_tree_node *n);

/*
 * send to sink
 */
size_t msg_tree_send(struct msg_tree *tree, struct ep_sink *s);


/*
 * apply from buffer
 */
void msg_tree_recv(struct ep_buffer *b, struct msg_tree *t);


#endif

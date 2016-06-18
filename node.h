#ifndef NODE_H
#define NODE_H

#include <stdint.h>


/*
 * names a specific recieving process
 */
struct node_dest {
    char proc_name [256];
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
    char      node_name [256];
};


struct node_src {
    struct node_base   node;
    int64_t            fd;
    struct node_dest  *sinks;
    int64_t            sinks_count;
    char               pipetype [256];
};


struct node_tree {
    struct node_base   node;
    struct node_base  *subnodes;
    int64_t            subnode_count;
};


struct node_buffer {
    struct node_dest   *dest_buf;
    struct node_src    *src_buf;
    struct node_tree   *tree_buf;
    int64_t             dest_count;
    int64_t             src_count;
    int64_t             tree_count;
    int64_t             max_index;
};



void init_node_buffer(struct node_buffer *buf, int64_t size);
void free_node_buffer(struct node_buffer *buf);
void resize_node_buffer(struct node_buffer *buf, int64_t size);

void register_node(struct node_buffer *buf);


#endif

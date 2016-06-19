#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include "mem.h"
#include "update.h"


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
    struct node_update_buffer   update_buf;
    struct mem_block            dest_buf;
    struct mem_block            src_buf;
    struct mem_block            tree_buf;
};


void init_node_buffer(struct node_buffer *buf);
void free_node_buffer(struct node_buffer *buf);

void update_node_buffer(struct node_buffer *buf);

void register_node_src(struct node_buffer *buf, struct node_src *src);


#endif

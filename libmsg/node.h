#ifndef NODE_H
#define NODE_H

#include <stdint.h>


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
    struct node_base   self;
    struct node_base  *subnodes;
    int64_t            size;
};


void msg_tree_init(struct msg_tree *t, char *hostname);
void msg_tree_add_proc(struct msg_tree *t, char *procname, size_t count);


#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"



int msg_tree_hash(struct ep_tree *t) {
    return t->count;
}


void msg_tree_init(struct ep_tree *t) {
    ep_tree_alloc(t, sizeof(struct msg_node), 1024);
}


void msg_tree_elems(struct ep_tree *t) {
    for (int i = 0; i < t->count; ++i) {
        struct ep_link *lk = &t->links[i];
        struct msg_node *n = ep_tree_find(t, lk->elem_id);
        printf("%d => %s (%d)\n", lk->elem_id, n->name, n->mode);

    }
}


struct msg_node *msg_tree_add_node(struct ep_tree *t, struct msg_node *p) {
    int newid = ep_tree_insert(t, 0);
    struct msg_node *newnode = ep_tree_find(t, newid);
    return newnode;
}


int msg_tree_add_proc(struct ep_tree *t, struct ep_buffer *procname, size_t count) {
    int newid = ep_tree_new_root(t);
    struct msg_node *node = ep_tree_find(t, newid);
    if (node) {
        memset(node->name, 0, 256);
        ep_buffer_peek(procname, node->name, 0, count);
        printf("adding %s\n", node->name);
    }
    return newid;
}


int msg_tree_subnode(struct ep_tree *t, struct ep_buffer *nodename, size_t count, int id) {
    int newid = ep_tree_insert(t, id);
    struct msg_node *node = ep_tree_find(t, newid);
    memset(node->name, 0, 256);
    ep_buffer_peek(nodename, node->name, 0, count);
    printf("adding %s\n", node->name);
    return newid;
}

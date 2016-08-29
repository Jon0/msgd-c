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


struct msg_node *msg_tree_add_node(struct ep_tree *t, struct msg_node *p) {
    int newid = ep_tree_insert(t, 0);
    struct msg_node *newnode = ep_tree_find(t, newid);
    return newnode;
}


void msg_tree_set_name(struct ep_tree *t, const char *hostname) {

    // create a root node
    ep_tree_link(t, -1, 0);
    struct msg_node *n = ep_tree_find(t, 0);
    strcpy(n->name, hostname);
    printf("[%s] init\n", n->name);
}


void msg_tree_add_proc(struct ep_tree *t, const char *procname, size_t count) {
    int id = ep_tree_insert(t, 0);
    struct msg_node *node = ep_tree_find(t, id);
    memset(node->name, 0, 256);
    memcpy(node->name, procname, count);
    printf("adding %s\n", node->name);
}

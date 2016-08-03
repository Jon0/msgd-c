#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"


void msg_tree_init(struct msg_tree *t, char *hostname) {
    strcpy(t->self.name, hostname);
    t->subnodes = malloc(sizeof(struct node_base) * 32);
    t->size = 0;
    printf("[%s] init\n", t->self.name);
}


void msg_tree_add_proc(struct msg_tree *t, char *procname, size_t count) {
    int index = t->size++;
    struct node_base *node = &t->subnodes[index];
    memset(node->name, 0, 256);
    memcpy(node->name, procname, count);
    printf("[%s] adding %s\n", t->self.name, node->name);
}

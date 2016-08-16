#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"


void msg_node_buffer_init(struct msg_node_buffer *buf) {
    buf->arr = malloc(sizeof(struct msg_tree_node) * 256);
    buf->size = 0;
    buf->avail = 256;
}


int msg_tree_hash(struct msg_tree *t) {
    return t->size;
}


void msg_tree_init(struct msg_tree *t, const char *hostname) {
    msg_node_buffer_init(&t->buf);
    strcpy(t->self.name, hostname);
    t->subnodes = malloc(sizeof(struct node_base) * 32);
    t->size = 0;
    printf("[%s] init\n", t->self.name);
}


void msg_tree_add_proc(struct msg_tree *t, const char *procname, size_t count) {
    int index = t->size++;
    struct node_base *node = &t->subnodes[index];
    memset(node->name, 0, 256);
    memcpy(node->name, procname, count);
    printf("[%s] adding %s\n", t->self.name, node->name);
}


void msg_read_tree(struct ep_buffer *b, struct msg_tree *tree) {
    msg_read_node(b, tree->root);
}


void msg_read_node(struct ep_buffer *b, struct msg_tree_node *n) {

}


void msg_write_tree(struct ep_buffer *b, struct msg_tree *tree) {
    msg_write_node(b, tree->root);
}


void msg_write_node(struct ep_buffer *b, struct msg_tree_node *n) {
    ep_buffer_insert(b, (char *) n, sizeof(struct msg_tree_node));

    // copy subnodes
    for (int i = 0; i < n->subnode_count; ++i) {
        msg_write_node(b, &n->subnodes[i]);
    }
}


void msg_tree_delta(struct ep_buffer *b, struct msg_tree *tree) {
    struct msg_delta_header h;
    int32_t read;

    // read header for total size
    read = ep_buffer_erase(b, (char *) &h, sizeof(struct msg_delta_header));
    printf("recv %d\n", read);

    msg_read_tree(b, tree);
}

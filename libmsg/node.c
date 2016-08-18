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


void msg_tree_print(struct msg_tree *t) {
    printf("tree structure:\n");
    printf("%s\n", t->self.name);
    printf("%d nodes\n", t->size);
    for (int i = 0; i < t->size; ++i) {

    }
}


void msg_tree_init(struct msg_tree *t) {
    msg_node_buffer_init(&t->buf);
    t->subnodes = malloc(sizeof(struct node_base) * 32);
    t->size = 0;
    memset(t->self.name, 0, 256);
}


void msg_tree_set_name(struct msg_tree *t, const char *hostname) {
    strcpy(t->self.name, hostname);
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


size_t msg_tree_send(struct msg_tree *tree, struct ep_sink *s) {
    struct msg_delta_header head;
    char buf [32];

    head.size = tree->size * 32;
    head.checksum = 0;

    // send delta of tree
    printf("sending tree struct (%d, %d)\n", s->epid, tree->size);
    ep_write_blk(s, (char *) &head, sizeof(struct msg_delta_header));
    for (int i = 0; i < tree->size; ++i) {
        memcpy(buf, tree->subnodes[i].name, 32);
        ep_write_blk(s, buf, 32);
    }
    return 0;
}


void msg_tree_recv(struct ep_buffer *b, struct msg_tree *tree) {
    struct msg_delta_header h;
    int32_t read;

    // read header for total size
    read = ep_buffer_erase(b, (char *) &h, sizeof(struct msg_delta_header));
    printf("recv %d\n", read);
    printf("size %d\n", h.size);
    printf("checksum %d\n", h.checksum);
    msg_read_tree(b, tree);
}

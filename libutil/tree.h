#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

#include "buffer.h"
#include "queue.h"


struct ep_link {
    int parent_id;
    int elem_id;
    int elem_index;
    int sub_count;
};


/*
 * elements and links are both packed continuously
 */
struct ep_tree {
    struct ep_link *links;
    char           *elems;
    size_t elem_size;
    size_t count;
    size_t avail;
    int next_id;
};


void ep_tree_alloc(struct ep_tree *t, size_t elem, size_t maxnodes);
int ep_tree_link(struct ep_tree *t, int parent, int index);
int ep_tree_new_root(struct ep_tree *t);
void *ep_tree_find(struct ep_tree *t, int id);
int ep_tree_insert(struct ep_tree *t, int id);

/*
 * remove node and all subnodes from the tree
 */
void ep_tree_remove(struct ep_tree *t, int id);


/*
 * read and write to streams
 */
int ep_tree_read(struct ep_tree *t, struct ep_buffer *b, size_t offset);
void ep_tree_write(struct ep_tree *t, struct ep_buffer *b);
void ep_tree_send(struct ep_tree *t, struct ep_sink *s);
size_t ep_tree_serial_bytes(struct ep_tree *t);

void ep_tree_print(struct ep_tree *t);
void ep_tree_print_rec(struct ep_link *l, int depth);

#endif

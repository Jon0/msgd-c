#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

#include "buffer.h"

struct ep_link {
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
    size_t elem_count;
    size_t link_count;
    size_t avail;
};


void ep_tree_alloc(struct ep_tree *t, size_t elem, size_t maxnodes);
void *ep_tree_find(struct ep_tree *t, int id);
void *ep_tree_insert(struct ep_tree *t, int id);
void ep_tree_remove(struct ep_tree *t, int id);


/*
 * read and write to streams
 */
void ep_tree_read(struct ep_tree *t, struct ep_buffer *b);
void ep_tree_write(struct ep_tree *t, struct ep_buffer *b);

#endif
#ifndef TREE_H
#define TREE_H

#include <stdlib.h>


struct ep_link {
    int node_id;
    int sub_count;
}


struct ep_tree {
    int   *links;
    void  *elems;
    size_t elem_size;
    size_t elem_count;
    size_t link_count;
    size_t avail;
};


void ep_tree_alloc(struct ep_tree *t, size_t elem, size_t maxnodes);
void *ep_tree_find(struct ep_tree *t, int id);
void *ep_tree_insert(struct ep_tree *t, int id);
void ep_tree_remove(struct ep_tree *t, int id);

#endif

#include "tree.h"


void ep_tree_alloc(struct ep_tree *t, size_t elemsize, size_t maxnodes) {
    size_t link_alloc = sizeof(int) * maxnodes;
    size_t elem_alloc = elemsize * maxnodes;
    size_t total_alloc = link_alloc + elem_alloc;
    char *mem = malloc(total_alloc);
    t->links = (struct ep_link *) mem;
    t->elems = &mem[link_alloc];
    t->elem_size = elemsize;
    t->elem_count = 0;
    t->link_count = 0;
    t->avail = maxnodes;
}


void *ep_tree_find(struct ep_tree *t, int id) {

}


void *ep_tree_insert(struct ep_tree *t, int id) {

}


void ep_tree_remove(struct ep_tree *t, int id) {

}

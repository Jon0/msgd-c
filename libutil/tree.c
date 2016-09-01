#include <stdio.h>
#include <string.h>

#include "tree.h"


void ep_tree_alloc(struct ep_tree *t, size_t elemsize, size_t maxnodes) {
    size_t link_alloc = sizeof(int) * maxnodes;
    size_t elem_alloc = elemsize * maxnodes;
    size_t total_alloc = link_alloc + elem_alloc;
    char *mem = malloc(total_alloc);
    t->links = (struct ep_link *) mem;
    t->elems = &mem[link_alloc];
    t->elem_size = elemsize;
    t->count = 0;
    t->avail = maxnodes;
    t->next_id = 0;
}


int ep_tree_link(struct ep_tree *t, int parent, int index) {
    int new_id = t->next_id++;
    int new_index = t->count++;

    // make space for the new element
    size_t movesize = sizeof(struct ep_link) * (t->count - index);
    memmove(&t->links[index + 1], &t->links[index], movesize);

    struct ep_link *lk = &t->links[index];
    lk->parent_id = parent;
    lk->elem_id = new_id;
    lk->elem_index = new_index;
    lk->sub_count = 0;
    return new_id;
}


void *ep_tree_find(struct ep_tree *t, int id) {

    // check each element
    for (int i = 0; i < t->count; ++i) {
        struct ep_link *lk = &t->links[i];
        if (lk->elem_id == id) {
            return &t->elems[t->elem_size * lk->elem_index];
        }
    }
    return NULL;
}


int ep_tree_insert(struct ep_tree *t, int id) {
    for (int i = 0; i < t->count; ++i) {
        struct ep_link *parent = &t->links[i];
        if (parent->elem_id == id) {
            int pid = parent->elem_id;
            int new_id = ep_tree_link(t, pid, i + 1);

            // the next element is along the same branch
            while (i >= 0 && pid != -1) {
                parent = &t->links[i];
                if (parent->elem_id == pid) {
                    pid = parent->parent_id;
                    ++parent->sub_count;
                }
                --i;
            }
            return new_id;
        }
    }
    return -1;
}


void ep_tree_remove(struct ep_tree *t, int id) {
    for (int i = 0; i < t->count; ++i) {
        struct ep_link *elem = &t->links[i];
        if (elem->elem_id == id) {
            int pid = elem->parent_id;

            // shift elements down
            size_t rmcount = elem->sub_count + 1;
            size_t movesize = sizeof(struct ep_link) * (t->count - i - rmcount);
            memmove(&t->links[i], &t->links[i + rmcount], movesize);
            t->count -= rmcount;

            // decrement counters
            while (i >= 0 && pid != -1) {
                elem = &t->links[i];
                if (elem->elem_id == pid) {
                    pid = elem->parent_id;
                    elem->sub_count -= rmcount;
                }
                --i;
            }
            return;
        }
    }
}


void ep_tree_read(struct ep_tree *t, struct ep_buffer *b) {
    ep_buffer_erase(b, (char *) &t->count, sizeof(size_t));

    // read each link
    printf("reading %u nodes\n", t->count);
    for (int i = 0; i < t->count; ++i) {
        ep_buffer_erase(b, (char *) &t->links[i], sizeof(struct ep_link));
        ep_buffer_erase(b, &t->elems[i * t->elem_size], t->elem_size);
    }
}


void ep_tree_write(struct ep_tree *t, struct ep_buffer *b) {
    // write count first
    ep_buffer_insert(b, (char *) &t->count, sizeof(size_t));

    // write each link
    for (int i = 0; i < t->count; ++i) {
        ep_buffer_insert(b, (char *) &t->links[i], sizeof(struct ep_link));
        ep_buffer_insert(b, &t->elems[i * t->elem_size], t->elem_size);
    }
}


void ep_tree_send(struct ep_tree *t, struct ep_sink *s) {
    // write count first
    ep_write_blk(s, (char *) &t->count, sizeof(size_t));

    // write each link
    printf("sending %u nodes\n", t->count);
    for (int i = 0; i < t->count; ++i) {
        ep_write_blk(s, (char *) &t->links[i], sizeof(struct ep_link));
        ep_write_blk(s, &t->elems[i * t->elem_size], t->elem_size);
    }
}


void ep_tree_print(struct ep_tree *t) {
    printf("%d nodes\n", t->count);
    if (t->count > 0) {
        ep_tree_print_rec(t->links, 0);
    }
}


void ep_tree_print_rec(struct ep_link *l, int depth) {
    for (int s = 0; s < depth; ++s) {
        printf("|");
    }
    if (l->sub_count) {
        printf("+");
    }
    else {
        printf("-");
    }
    printf("(%d)\n", l->elem_id);
    int i = 0;
    while (i < l->sub_count) {
        struct ep_link *sub = &l[i + 1];
        ep_tree_print_rec(sub, depth + 1);
        i += (sub->sub_count + 1);
    }
}

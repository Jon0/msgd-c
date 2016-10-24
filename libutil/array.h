#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdint.h>


typedef int32_t array_id_t;


/*
 * binary tree for storing chunks
 */
struct ep_array_store {
    void *left;
    void *right;
    size_t used;
    size_t avail;

};


int array_alloc(struct ep_array_store *s, size_t count);
int array_free(struct ep_array_store *s);


#endif

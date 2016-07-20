#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


/*
 * hash map attributes
 */
struct ep_hash_map {
    void *array;
    size_t elem_size;
    size_t elem_count;
    size_t array_max;
};


size_t ep_hash(int i);


void ep_map_insert(struct ep_hash_map *m, void *elem);


#endif

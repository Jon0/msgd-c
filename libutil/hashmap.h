#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


typedef size_t (*ep_hash_t)(void *);
typedef int (*ep_cmp_t)(void *, void *);


/*
 * hash map attributes
 */
struct ep_map {
    char     *array;
    ep_hash_t hashfn;
    size_t    elem_size;
    size_t    elem_count;
    size_t    array_max;
};


/*
 * map one key to many items
 */
struct ep_multimap {
    char     *array;
};


size_t ep_int_hash(int i);


void ep_map_alloc(struct ep_map *m, size_t esize, size_t count);
void ep_map_insert(struct ep_map *m, void *elem);
void ep_map_erase(struct ep_map *m, int i);
void *ep_map_get(struct ep_map *m, int i);


#endif

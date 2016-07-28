#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


/*
 * returns a unique identifier for elements
 */
typedef int (*ep_id_t)(void *);


/*
 * hash map attributes
 */
struct ep_map {
    char     *array;
    ep_id_t   idfn;
    size_t    elem_size;
    size_t    elem_count;
    size_t    array_max;
};


/*
 * a continuous section of the multimap
 */
struct ep_map_block {
    char *ptr;
    size_t elems;
};

/*
 * map one key to many items
 */
struct ep_multimap {
    char     *array;
    ep_id_t   idfn;
    size_t    elem_size;
    size_t    elem_count;
    size_t    array_max;
};


size_t ep_int_hash(int i);


void ep_map_alloc(struct ep_map *m, ep_id_t fn, size_t esize, size_t count);
void ep_map_free(struct ep_map *m);
int ep_map_insert(struct ep_map *m, void *elem);
int ep_map_erase(struct ep_map *m, int i);
void *ep_map_get(struct ep_map *m, int i);


#endif

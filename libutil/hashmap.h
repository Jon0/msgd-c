#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


/*
 * returns a unique identifier for elements
 */
typedef int (*ep_id_t)(void *);


/*
 * provides sized blocks of memory
 */
struct ep_memory {
    char     *array;
    size_t    elem_size;
    size_t    elem_count;
    size_t    array_max;
};


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
 * memory functions
 */
void ep_memory_init(struct ep_memory *m, size_t esize, size_t count);
void *ep_memory_alloc(struct ep_memory *m, size_t count);

/*
 * map functions
 */
size_t ep_int_hash(int i);
void ep_map_alloc(struct ep_map *m, ep_id_t fn, size_t esize, size_t count);
void ep_map_free(struct ep_map *m);
int ep_map_insert(struct ep_map *m, void *elem);
int ep_map_erase(struct ep_map *m, int elem);
void *ep_map_get(struct ep_map *m, int elem);


#endif

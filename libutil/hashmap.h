#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


/*
 * returns a unique identifier for elements
 */
typedef int (*ep_id_t)(void *);


/*
 * negative keys for unused space
 */
struct ep_pair  {
    int key;
    size_t count;
    void *value;
};


/*
 * provides sized blocks of memory
 */
struct ep_multimap {
    struct ep_pair *pairs;
    char           *values;
    size_t    elem_size;
    size_t    array_max;
    size_t    pair_count;
    size_t    value_count;
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
void ep_multimap_init(struct ep_multimap *m, size_t esize, size_t count);
void *ep_multimap_alloc(struct ep_multimap *m, int key, size_t count);

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

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>


/*
 * returns a unique identifier for elements
 */
typedef int (*ep_id_t)(void *);


/*
 * hash map attributes
 * requires a function to return int ids
 * TODO: how should the map check which space is free?
 */
struct ep_map {
    char     *array;
    ep_id_t   idfn;
    size_t    elem_size;
    size_t    elem_count;
    size_t    array_max;
};


/*
 * a portion of an array
 * end indicates the index of 1 element past the end
 */
struct ep_subarray  {
    int    key;
    short  begin;
    short  end;
};


/*
 * provides sized blocks of memory
 * mapping keys to subarrays
 */
struct ep_multimap {
    struct ep_map  keys;
    char          *values;
    size_t    elem_size;
    size_t    array_max;
    size_t    value_count;
};


/*
 * map functions
 */
size_t ep_int_hash(int i);
void ep_map_alloc(struct ep_map *m, ep_id_t fn, size_t esize, size_t count);
void ep_map_free(struct ep_map *m);
int ep_map_id(struct ep_map *m, int index);
int ep_map_insert(struct ep_map *m, void *elem);
int ep_map_erase(struct ep_map *m, int elem);
void *ep_map_get(struct ep_map *m, int elem);


/*
 * multimap functions
 */
void ep_multimap_init(struct ep_multimap *m, size_t esize, size_t count);
int ep_multimap_size(struct ep_multimap *m, int key);
void ep_multimap_create_key(struct ep_multimap *m, int key);

/*
 * return the starting index of the keys subarray
 */
int ep_multimap_insert(struct ep_multimap *m, int key, size_t count);
void *ep_multimap_get(struct ep_multimap *m, int key, int index);

#endif

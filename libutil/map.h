#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

#include "hash.h"


/*
 * hash pairs,
 * allow used elements to be stored continuously
 * negative index indicates free pair space
 */
struct msgu_key_pair {
    hash_t hash;
    int index;
};


/*
 * hash map attributes
 * requires functions for hashing and comparing elements
 * TODO store key value pairs like c++ unordered maps
 *
 * The id function returns and int key for each value.
 * Array stores values continuously allowing iterators
 * to check each element.
 */
struct msgu_map {
    struct msgu_key_pair *pair;
    char                 *array;
    msgu_map_hash_t       hash;
    msgu_map_cmp_t        cmp;
    size_t                key_size;
    size_t                data_size;
    size_t                elem_size;
    size_t                elem_count;
    size_t                array_max;
    size_t                chain_max;
};


/*
 * a portion of an array
 * end indicates the index of 1 element past the end
 */
struct msgu_subarray  {
    short  begin;
    short  end;
};


/*
 * provides sized blocks of memory
 * mapping keys to subarrays
 */
struct msgu_multimap {
    struct msgu_map  keys;
    char          *values;
    size_t    elem_size;
    size_t    array_max;
    size_t    value_count;
};


/*
 * map functions
 */
void msgu_map_init(struct msgu_map *m, msgu_map_hash_t hash, msgu_map_cmp_t cmp, size_t ksize, size_t vsize);
void msgu_map_alloc(struct msgu_map *m, size_t count);
void msgu_map_free(struct msgu_map *m);
hash_t msgu_map_id(struct msgu_map *m, int index);
int msgu_map_insert(struct msgu_map *m, void *key, void *elem);
int msgu_map_erase(struct msgu_map *m, void *key);
void *msgu_map_get(struct msgu_map *m, void *key);


/*
 * multimap functions
 */
void msgu_multimap_init(struct msgu_multimap *m, size_t esize, size_t count);
int msgu_multimap_size(struct msgu_multimap *m, int key);

/*
 * create new key and init to empty subarray
 */
void msgu_multimap_create_key(struct msgu_multimap *m, int key);


/*
 * get subarray by key
 */
struct msgu_subarray *msgu_multimap_get_key(struct msgu_multimap *m, int key);


/*
 * get subarray by index (0 to multimap value count)
 */
struct msgu_subarray *msgu_multimap_get_index(struct msgu_multimap *m, int index);


/*
 * return the starting index of the keys subarray
 */
int msgu_multimap_insert(struct msgu_multimap *m, int key, size_t count);
void *msgu_multimap_get(struct msgu_multimap *m, int key, int index);
void *msgu_multimap_get_value(struct msgu_multimap *m, int index);

#endif

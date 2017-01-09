#ifndef LIBUTIL_HASH_H
#define LIBUTIL_HASH_H

#include <stdlib.h>


typedef unsigned long hash_t;


/*
 * returns a hash for elements
 */
typedef hash_t (*msgu_map_hash_t)(const void *);

/*
 * returns 0 for equal elements
 */
typedef int (*msgu_map_cmp_t)(const void *, const void *);


/*
 * functions for int keys
 */
hash_t msgu_int_hash(const void *p);
int msgu_int_cmp(const void *a, const void *b);


/*
 * functions for uint32 keys
 */
hash_t msgu_uint32_hash(const void *p);
int msgu_uint32_cmp(const void *a, const void *b);


hash_t msgu_fast_hash(const void *data, size_t count);

#endif

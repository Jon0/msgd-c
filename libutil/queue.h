#ifndef LIBUTIL_QUEUE_H
#define LIBUTIL_QUEUE_H

#include "array.h"


/*
 * a continuous sequence of elements
 */
struct msgu_queue {
    struct msgu_array arr;
    size_t begin;
    size_t size;
};


void msgu_queue_init(struct msgu_queue *q, const struct msgu_element *fns, size_t elem_size);
void msgu_queue_alloc(struct msgu_queue *q, size_t max);


size_t msgu_queue_size(struct msgu_queue *q);
size_t msgu_queue_element_size(struct msgu_queue *q);
size_t msgu_queue_pop(struct msgu_queue *q, void *e, size_t count);
size_t msgu_queue_push(struct msgu_queue *q, void *e, size_t count);


/*
 * standard functions
 */
int msgu_queue_frag_read(struct msgu_stream *src, struct msgu_fragment *f, void *q);
int msgu_queue_frag_write(struct msgu_stream *dest, struct msgu_fragment *f, const void *q);
hash_t msgu_queue_map_hash(const void *q);
int msgu_queue_map_cmp(const void *a, const void *b);


static struct msgu_element msgu_queue_element = {
    .read  = msgu_queue_frag_read,
    .write = msgu_queue_frag_write,
    .hash  = msgu_queue_map_hash,
    .cmp   = msgu_queue_map_cmp,
};


#endif

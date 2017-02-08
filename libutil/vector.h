#ifndef LIBUTIL_QUEUE_H
#define LIBUTIL_QUEUE_H

#include "array.h"


/*
 * a continuous sequence of elements
 */
struct msgu_vector {
    struct msgu_array arr;
    uint64_t begin;
    uint64_t size;
};


void msgu_vector_init(struct msgu_vector *q, const struct msgu_type *fns, size_t elem_size);
void msgu_vector_alloc(struct msgu_vector *q, size_t max);


size_t msgu_vector_size(const struct msgu_vector *q);
size_t msgu_vector_element_size(const struct msgu_vector *q);
size_t msgu_vector_element_serial_size(const struct msgu_vector *q, size_t index);
size_t msgu_vector_pop(struct msgu_vector *q, void *e, size_t count);
size_t msgu_vector_push(struct msgu_vector *q, void *e, size_t count);


/*
 * standard functions
 */
size_t msgu_vector_frag_size(const void *q);
int msgu_vector_frag_read(struct msgu_stream *src, struct msgu_fragment *f, void *q);
int msgu_vector_frag_write(struct msgu_stream *dest, struct msgu_fragment *f, const void *q);
hash_t msgu_vector_map_hash(const void *q);
int msgu_vector_map_cmp(const void *a, const void *b);


static struct msgu_type msgu_vector_element = {
    .memory_size  = sizeof(struct msgu_vector),
    .serial_size  = msgu_vector_frag_size,
    .read  = msgu_vector_frag_read,
    .write = msgu_vector_frag_write,
    .hash  = msgu_vector_map_hash,
    .cmp   = msgu_vector_map_cmp,
};


#endif

#ifndef MSGUTIL_SERIAL_H
#define MSGUTIL_SERIAL_H

#include "hash.h"
#include "parser.h"


/*
 * functions for elements in arrays
 */
struct msgu_element {
    msgu_transfer_size_t  size;
    msgu_transfer_read_t  read;
    msgu_transfer_write_t write;
    msgu_map_hash_t       hash;
    msgu_map_cmp_t        cmp;
};


/*
 * read and write fixed size types
 * returns count for completed operations
 * return negative for partial operations or errors
 */
int msgu_read_fixed(struct msgu_stream *in, struct msgu_fragment *f, void *obj, size_t count);
int msgu_write_fixed(struct msgu_stream *out, struct msgu_fragment *f, const void *obj, size_t count);


/*
 * reads array of read functions object pointers
 */
int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_transfer_read_t *fns, void **objs, size_t count);
int msgu_write_many(struct msgu_stream *out, struct msgu_fragment *f, msgu_transfer_write_t *fns, const void **objs, size_t count);


/*
 * read write basic types
 */
int msgu_i32_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *i);
int msgu_i32_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *i);


#endif

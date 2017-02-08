#ifndef MSGUTIL_SERIAL_H
#define MSGUTIL_SERIAL_H

#include "hash.h"


struct msgu_stream;
struct msgu_fragment;


/*
 * transfer stream functions
 */
typedef size_t (*msgu_obj_size_t)(const void *);
typedef int (*msgu_obj_read_t)(struct msgu_stream *, struct msgu_fragment *, void *);
typedef int (*msgu_obj_write_t)(struct msgu_stream *, struct msgu_fragment *, const void *);


/*
 * print object to buffer
 */
typedef void (*msgu_obj_print_t)(char *, void *);


/*
 * basic information for each type
 */
struct msgu_type {
    size_t           memory_size;
    msgu_obj_size_t  serial_size;
    msgu_obj_read_t  read;
    msgu_obj_write_t write;
    msgu_obj_hash_t  hash;
    msgu_obj_cmp_t   cmp;
    msgu_obj_print_t print;
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
int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_obj_read_t *fns, void **objs, size_t count);
int msgu_write_many(struct msgu_stream *out, struct msgu_fragment *f, msgu_obj_write_t *fns, const void **objs, size_t count);


/*
 * read write basic types
 */
int msgu_i32_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *i);
int msgu_i32_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *i);


#endif

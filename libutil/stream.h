#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"


/*
 * types of read and write error
 */
enum msgu_stream_error {
    msgu_waiting = 0,
    msgu_remote_closed = -1,
    msgu_operation_error = -2,
    msgu_format_error = -3,
};


typedef int msgu_stream_id_t;


/*
 * functions for reading and writing to files / sockets
 * return 0 for no available input
 * return negative for errors and disconnections
 */
typedef ssize_t (*msgu_read_t)(msgu_stream_id_t, void *, size_t);
typedef ssize_t (*msgu_write_t)(msgu_stream_id_t, const void *, size_t);


/*
 * functions required to read and write objects
 */
struct msgu_stream_fn {
    msgu_read_t read;
    msgu_write_t write;
};


/*
 * buffers data
 */
struct msgu_stream {
    msgu_stream_id_t       id;
    struct msgu_stream_fn *fn;
    struct msgu_buffer     read_buf;
    struct msgu_buffer     write_buf;
};


void msgu_stream_init(struct msgu_stream *s, msgu_stream_id_t id, struct msgu_stream_fn *fn);
void msgu_stream_free(struct msgu_stream *s);


/*
 * return number of bytes read, or zero for no available input
 * -1 indicates errors
 */
ssize_t msgu_stream_read(struct msgu_stream *s, void *buf, size_t count);
ssize_t msgu_stream_write(struct msgu_stream *s, const void *buf, size_t count);
ssize_t msgu_stream_discard(struct msgu_stream *s, size_t count);


/*
 * shows size and write progress
 */
struct msgu_fragment {
    size_t index;
    size_t count;
};


/*
 * fragment stream functions
 */
typedef int (*msgu_frag_read_t)(struct msgu_stream *, struct msgu_fragment *, void *);
typedef int (*msgu_frag_write_t)(struct msgu_stream *, struct msgu_fragment *, const void *);


/*
 *
 */
void msgu_fragment_reset(struct msgu_fragment *f, size_t count);
size_t msgu_fragment_progress(struct msgu_fragment *f);
size_t msgu_fragment_advance(struct msgu_fragment *f, size_t count);
void msgu_fragment_inc(struct msgu_fragment *f);


/*
 * return 1 for no errors
 */
int msgu_fragment_check(struct msgu_fragment *f, int result);


/*
 * check at least count bytes have been transferred
 */
int msgu_fragment_complete(struct msgu_fragment *f, int result, size_t count);


/*
 * read and write fixed size types
 */
int msgu_read_fixed(struct msgu_stream *in, struct msgu_fragment *f, void *obj, size_t count);
int msgu_write_fixed(struct msgu_stream *out, struct msgu_fragment *f, const void *obj, size_t count);


/*
 * reads array of read functions object pointers
 */
int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_frag_read_t *fns, void **objs, size_t count);
int msgu_write_many(struct msgu_stream *out, struct msgu_fragment *f, msgu_frag_write_t *fns, const void **objs, size_t count);


/*
 * read write basic types
 */
int msgu_i32_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *i);
int msgu_i32_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *i);



#endif

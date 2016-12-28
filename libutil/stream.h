#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"


typedef int msgu_stream_id_t;


/*
 * functions for reading and writing to files / sockets
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
ssize_t msgu_stream_read(struct msgu_stream *s, void *buf, size_t count);
ssize_t msgu_stream_write(struct msgu_stream *s, const void *buf, size_t count);

#endif

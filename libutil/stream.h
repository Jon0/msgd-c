#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"


/*
 * Types of read and write errors.
 *
 * Direct read write sources include number of bytes
 * instead of complete / partial codes.
 * Fragment sources only return one of these.
 */
enum msgu_stream_error {
    msgu_stream_complete        = 1,
    msgu_stream_partial         = 0,
    msgu_stream_waiting         = -1,
    msgu_stream_remote_closed   = -2,
    msgu_stream_operation_error = -3,
    msgu_stream_format_error    = -4,
    msgu_stream_not_open        = -5,
};


enum msgu_stream_state_bits {
    msgu_stream_open            = 0x01,
    msgu_stream_end_of_read     = 0x02,
    msgu_stream_end_of_write    = 0x04,
};


/*
 * used to distinguish streams
 */
union msgu_stream_id {
    void *ptr;
    int   fd;
};


typedef union msgu_stream_id msgu_stream_id_t;


/*
 * functions for reading and writing to files / sockets
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
 * use buffer as a stream source
 */
ssize_t msgu_buffer_read_fn(msgu_stream_id_t id, void *buf, size_t count);
ssize_t msgu_buffer_write_fn(msgu_stream_id_t id, const void *buf, size_t count);


static struct msgu_stream_fn msgu_buffer_stream = {
    .read  = msgu_buffer_read_fn,
    .write = msgu_buffer_write_fn,
};


/*
 * reads and writes files and sockets
 * checks for end of file and socket disconnects
 */
struct msgu_stream {
    struct msgu_stream_fn *fn;
    msgu_stream_id_t       id;
    int                    state;
};


void msgu_stream_init(struct msgu_stream *s, struct msgu_stream_fn *fn, msgu_stream_id_t id);
void msgu_stream_free(struct msgu_stream *s);


/*
 * check stream is open
 */
int msgu_stream_is_open(const struct msgu_stream *stream);


/*
 * return number of bytes read, or zero for no available input
 * -1 indicates errors
 */
ssize_t msgu_stream_read(struct msgu_stream *s, void *buf, size_t count);
ssize_t msgu_stream_write(struct msgu_stream *s, const void *buf, size_t count);
ssize_t msgu_stream_discard(struct msgu_stream *s, size_t count);


#endif

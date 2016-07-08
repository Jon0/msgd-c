#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <poll.h>

#include "endpoint.h"


struct ep_buffer {
    char *ptr;
    size_t avail;
    size_t begin;
    size_t size;
};


struct ep_read_data {
    struct ep_address *srcaddr;
    struct ep_buffer   buf;
    void              *notify_obj;
    notify_fn_t        notify_read;
};


/*
 * give the buffer some memory
 */
void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count);

/*
 * return end index and continuous bytes that can be used past the end
 */
void ep_buffer_endmem(struct ep_buffer *b, char **end, size_t *space);

/*
 * push data into the buffer
 */
ssize_t ep_buffer_insert(struct ep_buffer *b, const char *inbuf, size_t count);

/*
 * take available data from a readable file descriptor
 */
ssize_t ep_buffer_take(struct ep_buffer *b, int fd);
size_t ep_buffer_take_src(struct ep_buffer *b, struct ep_source *s, size_t count);

/*
 * write data to a file descriptor, make no changes to the buffer
 */
ssize_t ep_buffer_write(struct ep_buffer *b, struct ep_dest *d, size_t begin);
size_t ep_buffer_write_inc(struct ep_buffer *b, struct ep_dest *d, size_t *begin);

/*
 * increment the buffer front, as data is no longer needed
 */
void ep_buffer_release(struct ep_buffer *b, size_t count);

/*
 * threaded component for the reader
 */
void *ep_thread_read(void *p);

/*
 * just create threads, the attributes should be initialised
 */
void ep_create_reader(struct ep_address *s, notify_fn_t fn, void *obj);


#endif

#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <poll.h>

#include "endpoint.h"


struct ep_buffer {
    char *ptr;
    size_t avail;
    size_t begin;
    size_t end;
};


struct ep_read_data {
    struct pollfd event;
    struct ep_buffer *buf;
};


/*
 * give the buffer some memory
 */
void ep_buffer_init(struct ep_buffer *b, void *mem, size_t count);

/*
 * write data to a file descriptor, make no changes to the buffer
 */
ssize_t ep_buffer_write(const struct ep_buffer *b, int fd, size_t begin);

/*
 * take available data from a readable file descriptor
 */
void ep_buffer_take(struct ep_buffer *b, int fd);

/*
 * increment the buffer front, as data is no longer needed
 */
void ep_buffer_release(struct ep_buffer *b, size_t count);

void *ep_thread_read(void *p);

void ep_create_reader(struct ep_table *t, int fd);

#endif

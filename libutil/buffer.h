#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>

#define EP_BUFFER_DEFAULT_SIZE 4096


/*
 * a continuous sequence of elements
 * forming either an array or queue
 * implemented using a ring buffer
 */
struct msgu_buffer {
    char *ptr;
    size_t avail;
    size_t begin;
    size_t size;
    size_t esize;
};


typedef size_t (*ep_buffer_read_t)(struct msgu_buffer *, size_t, void *);
typedef size_t (*ep_buffer_write_t)(struct msgu_buffer *, void *);


/*
 * many blocks may refer to a single buffer
 */
struct msgu_block {
    char *first;
    char *second;
    size_t first_count;
    size_t second_count;
};


/*
 * give the buffer some memory
 */
void msgu_buffer_init_default(struct msgu_buffer *b);
void msgu_buffer_init(struct msgu_buffer *b, void *mem, size_t count);


/*
 * wraps a fixed size buffer
 */
void ep_buffer_wrap(struct msgu_buffer *b, char *buf, size_t count);


/*
 * modify buffer size and keeping existing data
 */
void ep_buffer_resize(struct msgu_buffer *b, size_t newsize);
void ep_buffer_free(struct msgu_buffer *b);


/*
 * return end index and continuous bytes that can be used past the end
 */
void ep_buffer_endmem(struct msgu_buffer *b, char **end, size_t *space);


/*
 * remove all content from the buffer
 */
void ep_buffer_clear(struct msgu_buffer *b);

/*
 * copy from one buffer to another
 */
size_t ep_buffer_copy(struct msgu_buffer *outbuf, struct msgu_buffer *inbuf, size_t start);

/*
 * push data into the buffer
 */
size_t ep_buffer_insert(struct msgu_buffer *b, const char *inbuf, size_t count);


/*
 * take bytes from the start of the buffer, but do not erase
 */
size_t ep_buffer_peek(struct msgu_buffer *b, char *outbuf, size_t offset, size_t count);


/*
 * take bytes from the start of the buffer, and erase from buffer
 */
size_t ep_buffer_erase(struct msgu_buffer *b, char *outbuf, size_t count);

/*
 * take available data from a readable file descriptor
 */
ssize_t ep_buffer_take(struct msgu_buffer *b, int fd);
size_t ep_buffer_take_src(struct msgu_buffer *b, int fd, size_t count);

/*
 * write data to a file descriptor, make no changes to the buffer
 */
ssize_t ep_buffer_write(struct msgu_buffer *b, int fd, size_t begin);
size_t ep_buffer_write_inc(struct msgu_buffer *b, int fd, size_t *begin);

/*
 * increment the buffer front, as data is no longer needed
 */
void ep_buffer_release(struct msgu_buffer *b, size_t count);

#endif

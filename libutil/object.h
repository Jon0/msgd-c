#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"


typedef uint32_t ep_size_t;


typedef size_t (*msgu_read_t)(struct ep_buffer *, size_t, void *);
typedef size_t (*msgu_write_t)(struct ep_buffer *, void *);


/*
 * functions required to read and write objects
 */
struct msgu_fd {
    msgu_read_t read;
    msgu_write_t write;
};


void ep_string_read(struct ep_buffer *b, char *out, size_t count);
void ep_string_write(struct ep_buffer *b, char *in, size_t count);


#endif

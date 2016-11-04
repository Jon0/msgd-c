#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"


typedef uint32_t ep_size_t;

typedef int (*ep_serial_hash_t)(void *);
typedef size_t (*ep_serial_size_t)(void *);
typedef size_t (*ep_serial_read_t)(struct ep_buffer *, size_t, void *);
typedef size_t (*ep_serial_write_t)(struct ep_buffer *, void *);

/*
 * set of functions required to read and write objects
 */
struct ep_object {
    ep_serial_size_t hash;
    ep_serial_size_t size;
    ep_serial_read_t read;
    ep_serial_write_t write;
};


void ep_string_read(struct ep_buffer *b, char *out, size_t count);
void ep_string_write(struct ep_buffer *b, char *in, size_t count);


#endif

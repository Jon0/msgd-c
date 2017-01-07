#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include <stdint.h>

#include "stream.h"


struct msgu_string {
    uint32_t   count;
    char      *buf;
};


void msgu_string_init(struct msgu_string *str);
void msgu_string_alloc(struct msgu_string *str, size_t size);


/*
 * length of string data including header
 */
size_t msgu_string_size(struct msgu_string *str);
ssize_t msgu_string_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_string *str);
ssize_t msgu_string_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_string *str);


int msgu_string_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *str);
int msgu_string_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *str);


struct msgu_string_array {
    uint32_t    substrs;
    uint32_t    total_bytes;
    char       *buf;
};


#endif

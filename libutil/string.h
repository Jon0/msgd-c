#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include <stdint.h>

#include "stream.h"


struct msgu_string {
    uint32_t    count;
    char *buf;
};


void msgu_string_init(struct msgu_string *str);
void msgu_string_alloc(struct msgu_string *str, size_t size);


/*
 * length of string data including header
 */
size_t msgu_string_size(struct msgu_string *str);
ssize_t msgu_string_read(struct msgu_string *str, struct msgu_stream *src, size_t off);
ssize_t msgu_string_write(struct msgu_string *str, struct msgu_stream *dest, size_t off);


#endif

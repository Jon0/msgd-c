#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include <stdint.h>

#include "array.h"
#include "hash.h"
#include "parser.h"
#include "stream.h"


/*
 * array of single byte elements
 */
struct msgu_string {
    uint32_t  count;
    char     *buf;
};


void msgu_string_init(struct msgu_string *str);
void msgu_string_alloc(struct msgu_string *str, size_t size);
void msgu_string_free(struct msgu_string *str);
void msgu_string_from_static(struct msgu_string *str, const char *cstr);
void msgu_string_from_buffer(struct msgu_string *str, const char *cstr, size_t count);
void msgu_string_copy(struct msgu_string *dest, const struct msgu_string *src);
size_t msgu_string_split(struct msgu_string *dest, size_t count, const char *src, const char *delim);


/*
 * compare / order strings
 */
int msgu_string_compare(const struct msgu_string *a, const struct msgu_string *b);


/*
 * length of string data including header
 */
size_t msgu_string_size(const struct msgu_string *str);
int msgu_string_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_string *str);
int msgu_string_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_string *str);


/*
 * standard functions for strings
 */
size_t msgu_string_size_frag(const void *str);
int msgu_string_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *str);
int msgu_string_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *str);
hash_t msgu_string_hash(const void *p);
int msgu_string_cmp(const void *a, const void *b);


static struct msgu_type msgu_string_element = {
    .memory_size  = sizeof(struct msgu_string),
    .serial_size  = msgu_string_size_frag,
    .read  = msgu_string_read_frag,
    .write = msgu_string_write_frag,
    .hash  = msgu_string_hash,
    .cmp   = msgu_string_cmp,
};


#endif

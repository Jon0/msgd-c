#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include <stdint.h>

#include "array.h"
#include "hash.h"
#include "stream.h"


/*
 * array of single byte elements
 */
struct msgu_string {
    uint32_t   count;
    char      *buf;
};


void msgu_string_init(struct msgu_string *str);
void msgu_string_alloc(struct msgu_string *str, size_t size);

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


int msgu_string_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *str);
int msgu_string_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *str);


/*
 * hash functions for strings
 */
hash_t msgu_string_hash(const void *p);
int msgu_string_cmp(const void *a, const void *b);


struct msgu_string_array {
    uint32_t    substrs;
    uint32_t    total_bytes;
    char       *buf;
};


#endif

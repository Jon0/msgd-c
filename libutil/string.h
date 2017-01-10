#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include <stdint.h>

#include "array.h"
#include "hash.h"
#include "stream.h"


/*
 * continuous set of elements
 */
struct msgu_list {
    struct msgu_array data;
    size_t count;
};


void msgu_list_init(struct msgu_list *list, size_t elem_size);
void msgu_list_alloc(struct msgu_list *list, size_t max);
void msgu_list_free(struct msgu_list *list);


size_t msgu_list_size(struct msgu_list *list);
int msgu_array_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_list *list);
int msgu_array_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_list *list);


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
 * length of string data including header
 */
size_t msgu_string_size(struct msgu_string *str);
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

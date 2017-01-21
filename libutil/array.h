#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdint.h>

#include "serial.h"


typedef int32_t array_id_t;


struct msgu_array {
    const struct msgu_element *fns;
    char *data;
    size_t esize;
    size_t allocated;
};


void msgu_array_init(struct msgu_array *a, const struct msgu_element *fns, size_t elem_size);
void msgu_array_alloc(struct msgu_array *a, size_t max);
void msgu_array_free(struct msgu_array *a);
void *msgu_array_access(struct msgu_array *a, size_t index);


/*
 * read or write to a subarray
 */
size_t msgu_array_get(struct msgu_array *a, size_t index, void *elem, size_t count);
size_t msgu_array_set(struct msgu_array *a, size_t index, void *elem, size_t count);


/*
 * wrap back to beginning if end of array is reached
 */
size_t msgu_array_get_wrap(struct msgu_array *a, size_t index, void *elem, size_t count);
size_t msgu_array_set_wrap(struct msgu_array *a, size_t index, void *elem, size_t count);

/*
 * move data to another location in the same array
 */
size_t msgu_array_move(struct msgu_array *a, size_t dest, size_t src, size_t count);


size_t msgu_array_serial_size(const struct msgu_array *array, size_t start, size_t count);
int msgu_array_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_array *array, size_t start, size_t count);
int msgu_array_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_array *array, size_t start, size_t count);


#endif

#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdint.h>


typedef int32_t array_id_t;


struct msgu_array {
    char *data;
    size_t esize;
    size_t count;
    size_t allocated;
};


void msgu_array_init(struct msgu_array *a, size_t elem_size);
void msgu_array_alloc(struct msgu_array *a, size_t max);
void msgu_array_free(struct msgu_array *a);


int msgu_array_get(struct msgu_array *a, size_t index, void *elem, size_t count);
int msgu_array_set(struct msgu_array *a, size_t index, void *elem, size_t count);
int msgu_array_get_wrap(struct msgu_array *a, size_t index, void *elem, size_t count);
int msgu_array_set_wrap(struct msgu_array *a, size_t index, void *elem, size_t count);
int msgu_array_move(struct msgu_array *a, size_t dest, size_t src, size_t count);


#endif

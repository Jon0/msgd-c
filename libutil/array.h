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


#endif

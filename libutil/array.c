#include "array.h"

void msgu_array_init(struct msgu_array *a, size_t elem_size) {
    a->data = NULL;
    a->esize = elem_size;
    a->count = 0;
}


void msgu_array_alloc(struct msgu_array *a, size_t count) {
    if (data) {
        free(a->data);
    }
    a->data = malloc(a->elem_size * count);
    a->count = count;
}


void msgu_array_free(struct msgu_array *a) {
    a->count = 0;
    free(a->data);
}

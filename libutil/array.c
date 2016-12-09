#include "array.h"

void msgu_array_init(struct msgu_array *a, size_t elem_size) {
    a->data = NULL;
    a->esize = elem_size;
    a->count = 0;
    a->allocated = 0;
}


void msgu_array_alloc(struct msgu_array *a, size_t max) {
    if (a->data) {
        free(a->data);
    }
    a->data = malloc(a->esize * max);
    a->allocated = max;
}


void msgu_array_free(struct msgu_array *a) {
    a->allocated = 0;
    free(a->data);
}


int msgu_array_insert(struct msgu_array *a, size_t index, void *element) {
    if (index > a->count || a->count == a->allocated) {
        return 0;
    }
    size_t movesize = a->esize * (a->count - index);
    size_t endindex = a->esize * index;
    memmove(&a->data[endindex + a->esize], &a->data[endindex], movesize);
    memmove(&a->data[endindex], element, a->esize);
    return 1;
}

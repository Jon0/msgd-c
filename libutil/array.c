#include <string.h>

#include "array.h"

void msgu_array_init(struct msgu_array *a, size_t elem_size) {
    a->data = NULL;
    a->esize = elem_size;
    a->allocated = 0;
}


void msgu_array_alloc(struct msgu_array *a, size_t max) {
    char *mem = malloc(a->esize * max);
    if (a->data) {
        memmove(mem, a->data, a->allocated);
        free(a->data);
    }
    a->allocated = max;
}


void msgu_array_free(struct msgu_array *a) {
    a->allocated = 0;
    free(a->data);
}


int msgu_array_get(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if ((index + count) > a->allocated) {
        return 0;
    }
    memmove(elem, &a->data[a->esize * index], a->esize * count);
    return count;
}


int msgu_array_set(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if ((index + count) > a->allocated) {
        return 0;
    }
    memmove(&a->data[a->esize * index], elem, a->esize * count);
    return count;
}


int msgu_array_get_wrap(struct msgu_array *a, size_t index, void *elem, size_t count) {

}


int msgu_array_set_wrap(struct msgu_array *a, size_t index, void *elem, size_t count) {

}


int msgu_array_move(struct msgu_array *a, size_t dest, size_t src, size_t count) {
    if ((dest + count) > a->allocated || (src + count) > a->allocated) {
        return 0;
    }
    memmove(&a->data[a->esize * dest], &a->data[a->esize * src], a->esize * count);
    return count;
}

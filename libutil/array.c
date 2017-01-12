#include <string.h>

#include "array.h"


void msgu_array_init(struct msgu_array *a, const struct msgu_element *fns, size_t elem_size) {
    a->fns = fns;
    a->esize = elem_size;
    a->data = NULL;
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


size_t msgu_array_get(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if ((index + count) > a->allocated) {
        count = a->allocated - index;
    }
    memmove(elem, &a->data[a->esize * index], a->esize * count);
    return count;
}


size_t msgu_array_set(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if ((index + count) > a->allocated) {
        count = a->allocated - index;
    }
    memmove(&a->data[a->esize * index], elem, a->esize * count);
    return count;
}


size_t msgu_array_get_wrap(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if (count > a->allocated) {
        count = a->allocated;
    }
    size_t block1 = a->allocated - index;
    if (count > block1) {
        memmove(elem, &a->data[a->esize * index], a->esize * block1);
        memmove(&((char *) elem)[a->esize * block1], a->data, a->esize * (count - block1));
    }
    else {
        memmove(elem, &a->data[a->esize * index], a->esize * count);
    }
    return count;
}


size_t msgu_array_set_wrap(struct msgu_array *a, size_t index, void *elem, size_t count) {
    if (count > a->allocated) {
        count = a->allocated;
    }
    size_t block1 = a->allocated - index;
    if (count > block1) {
        memmove(&a->data[a->esize * index], elem, a->esize * block1);
        memmove(a->data, &((char *) elem)[a->esize * block1], a->esize * (count - block1));
    }
    else {
        memmove(&a->data[a->esize * index], elem, a->esize * count);
    }
    return count;
}


size_t msgu_array_move(struct msgu_array *a, size_t dest, size_t src, size_t count) {
    if ((dest + count) > a->allocated || (src + count) > a->allocated) {
        return 0;
    }
    memmove(&a->data[a->esize * dest], &a->data[a->esize * src], a->esize * count);
    return count;
}


size_t msgu_array_serial_size(const struct msgu_array *array, size_t start, size_t count) {
    size_t arr_size = 0;
    for (int i = 0; i < count; ++i) {
        size_t index = (start + i) % array->allocated;
        arr_size += array->fns->size(&array->data[array->esize * i]);
    }
    return arr_size;
}


int msgu_array_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_array *array, size_t start, size_t count) {
    for (int i = f->index; i < count; ++i) {
        size_t arr_index = (start + i) % array->allocated;
        int result = array->fns->read(src, &f[1], &array->data[array->esize * arr_index]);
        if (result > 0) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return 1;
}


int msgu_array_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_array *array, size_t start, size_t count) {
    for (int i = f->index; i < count; ++i) {
        size_t arr_index = (start + i) % array->allocated;
        int result = array->fns->write(dest, &f[1], &array->data[array->esize * arr_index]);
        if (result > 0) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return 1;
}

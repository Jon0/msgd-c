#include <string.h>

#include "datatable.h"


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count) {
    t->count = count;
    t->sizes = malloc(sizeof(size_t) * count);
    memcpy(t->sizes, sizes, sizeof(size_t) * count);
    t->rowsize = 0;
    for (int i = 0; i < count; ++i) {
        t->rowsize += t->sizes[i];
    }
}


void msgu_datatable_alloc(struct msgu_datatable *t, size_t max) {
    t->data = malloc(t->rowsize * max);
    t->allocated = max;
}


int msgu_datatable_emplace(struct msgu_datatable *t, void **table, size_t count) {
    return 0;
}

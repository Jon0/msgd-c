#include <string.h>

#include "datatable.h"


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count) {
    t->sizes_count = count;
    t->sizes = malloc(sizeof(size_t) * count);
    memcpy(t->sizes, sizes, sizeof(size_t) * count);
    t->rowsize = 0;
    for (int i = 0; i < count; ++i) {
        t->rowsize += t->sizes[i];
    }
    t->row_count = 0;
}


void msgu_datatable_alloc(struct msgu_datatable *t, size_t max) {
    t->data = malloc(t->rowsize * max);
    t->allocated = max;
}


int msgu_datatable_emplace(struct msgu_datatable *t, void **table, size_t count) {
    size_t index = t->row_count++;
    size_t char_index = t->rowsize * index;
    for (int i = 0; i < t->sizes_count; ++i) {
        table[i] = t->data[char_index];
        char_index += t->sizes[i];
    }
    return index;
}


int msgu_datatable_create_key(struct msgu_datamap *m, int index) {
    return 0;
}

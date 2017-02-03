#include <string.h>

#include "datatable.h"


void msgu_datatable_init(struct msgu_datatable *t, size_t *sizes, size_t count) {
    t->data        = NULL;
    t->row_count   = 0;
    t->sizes_count = count;
    t->sizes       = malloc(sizeof(size_t) * count);
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


int msgu_datatable_get(struct msgu_datatable *t, void **table, size_t index) {
    size_t char_index = t->rowsize * index;
    for (int i = 0; i < t->sizes_count; ++i) {
        table[i] = &t->data[char_index];
        char_index += t->sizes[i];
    }
    return 0;
}


int msgu_datatable_emplace(struct msgu_datatable *t, void **table) {
    size_t index = t->row_count++;
    msgu_datatable_get(t, table, index);
    return index;
}


void msgu_datamap_init(struct msgu_datamap *m, struct msgu_datatable *t, size_t item, msgu_map_hash_t hash, msgu_map_cmp_t cmp) {
    m->table = t;
    m->item  = item;
    m->hash  = hash;
    m->cmp   = cmp;
    m->alloc = 0;
    m->chain = 8;
    m->data  = NULL;
}


void msgu_datamap_alloc(struct msgu_datamap *m, size_t bits) {
    m->data = malloc(bits);
    m->alloc = bits;
}


int msgu_datamap_create_key(struct msgu_datamap *m, int index) {
    return 0;
}

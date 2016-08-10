#include <string.h>

#include "hashmap.h"


void ep_memory_init(struct ep_memory *m, size_t esize, size_t count) {
    m->elem_count = 0;
    m->elem_size = esize;
    m->array_max = count;
    size_t memsize = m->elem_size * m->array_max;
    m->array = malloc(memsize);
    memset(m->array, 0, memsize);
}


void *ep_memory_alloc(struct ep_memory *m, size_t count) {
    void *result = &m->array[m->elem_count * m->elem_size];
    m->elem_count += count;
    return result;
}



size_t ep_int_hash(int i) {
    return i;
}


void ep_map_alloc(struct ep_map *m, ep_id_t fn, size_t esize, size_t count) {
    m->elem_count = 0;
    m->idfn = fn;
    m->elem_size = esize;
    m->array_max = count;
    size_t memsize = m->elem_size * m->array_max;
    m->array = malloc(memsize);
    memset(m->array, 0, memsize);
}


void ep_map_free(struct ep_map *m) {
    free(m->array);
}


int ep_map_insert(struct ep_map *m, void *elem) {
    size_t array_pos = ep_int_hash(m->idfn(elem)) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = ((array_pos + i) % m->array_max) * m->elem_size;
        void *item = &m->array[index];
        if (m->idfn(item) == 0) {
            memcpy(item, elem, m->elem_size);
            return 1;
        }
    }
    return 0;
}


int ep_map_erase(struct ep_map *m, int elem) {
    return 0;
}


void *ep_map_get(struct ep_map *m, int elem) {
    size_t array_pos = ep_int_hash(elem) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = ((array_pos + i) % m->array_max) * m->elem_size;
        void *item = &m->array[index];
        if (m->idfn(item) == elem) {
            return item;
        }
    }
    return NULL;
}

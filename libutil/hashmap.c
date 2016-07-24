#include "hashmap.h"


size_t ep_hash(int i) {
    return i;
}


void ep_map_alloc(struct ep_map *m, size_t esize, size_t count) {
    m->elem_count = 0;
    m->elem_size = esize;
    m->array_max = count;
    m->array = malloc(m->elem_size * m->array_max);
}


void ep_map_insert(struct ep_map *m, void *elem) {
    size_t array_pos = m->hashfn(elem) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = ((array_pos + i) % m->array_max) * m->elem_size;
        void *item = &m->array[index];

        //
        if (item->epid == 0) {

        }
    }
    printf("array is full\n");
    return NULL;
}


void ep_map_erase(struct ep_map *m, int i) {

}




void *ep_map_get(struct ep_map *m, int i) {
    size_t array_pos = m->hashfn(elem) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = ((array_pos + i) % m->array_max) * m->elem_size;
        void *item = &m->array[index];
        if (item->epid == epid) {
            return item;
        }
    }
    printf("epid not found: %d\n", epid);
    return NULL;
}

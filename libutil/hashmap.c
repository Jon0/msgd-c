#include <string.h>

#include "hashmap.h"


int ep_subarray_key(void *p) {
    struct ep_subarray *sa = p;
    return sa->key;
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
    size_t pairsize = sizeof(struct ep_keypair) * m->array_max;
    char *mem = malloc(memsize + pairsize);
    memset(mem, 0, memsize + pairsize);
    m->pair = (struct ep_keypair *) mem;
    m->array = &mem[pairsize];
}


void ep_map_free(struct ep_map *m) {
    free(m->array);
}


int ep_map_id(struct ep_map *m, int index) {
    void *item = &m->array[index * m->elem_size];
    return m->idfn(item);
}


int ep_map_insert(struct ep_map *m, void *elem) {
    size_t array_pos = ep_int_hash(m->idfn(elem)) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct ep_keypair *keypair = &m->pair[index];
        if (keypair->index < 0) {
            void *item = &m->array[keypair->index * m->elem_size];
            memcpy(item, elem, m->elem_size);
            ++m->elem_count;
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


void ep_multimap_init(struct ep_multimap *m, size_t esize, size_t count) {
    ep_map_alloc(&m->keys, ep_subarray_key, sizeof(struct ep_subarray), count);
    m->value_count = 0;
    m->elem_size = esize;
    m->array_max = count;
    size_t value_mem = m->elem_size * m->array_max;
    m->values = malloc(value_mem);
    memset(m->values, 0, value_mem);
}


int ep_multimap_size(struct ep_multimap *m, int key) {
    struct ep_subarray *arr = ep_map_get(&m->keys, key);
    if (arr == NULL) {
        return 0;
    }
    return arr->end - arr->begin;
}


void ep_multimap_create_key(struct ep_multimap *m, int key) {
    struct ep_subarray new_sa;
    new_sa.key = key;
    new_sa.begin = 0;
    new_sa.end = 0;
    ep_map_insert(&m->keys, &new_sa);
}


int ep_multimap_insert(struct ep_multimap *m, int key, size_t count) {
    struct ep_subarray *arr = ep_map_get(&m->keys, key);
    if (arr == NULL) {
        ep_multimap_create_key(m, key);
        arr = ep_map_get(&m->keys, key);
    }

    // move elements to make space
    size_t newsize = count * m->elem_size;
    size_t movesize = m->elem_size * (m->value_count - arr->end);
    memmove(&m->values[arr->end + newsize], &m->values[arr->end], movesize);

    // increment all other subarrays
    for (int i = 0; i < m->keys.elem_count; ++i) {
        if (ep_map_id(&m->keys, i) != 0) {
            struct ep_subarray *isa = (struct ep_subarray *) &m->keys.array[i * m->keys.elem_size];
            if (isa->begin >= arr->end) {
                isa->begin += count;
                isa->end += count;
            }
        }
    }
    int result = arr->end;
    arr->end += count;
    m->value_count += count;
    return result;
}


void *ep_multimap_get(struct ep_multimap *m, int key, int index) {
    struct ep_subarray *arr = ep_map_get(&m->keys, key);
    if (arr == NULL) {
        return NULL;
    }

    // check index is within bounds
    int arr_size = arr->end - arr->begin;
    if (0 > index || index >= arr_size) {
        return NULL;
    }

    return &m->values[(arr->begin + index) * m->elem_size];
}

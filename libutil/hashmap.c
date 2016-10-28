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

    // allocate memory for 2 arrays
    size_t pairsize = sizeof(struct ep_keypair) * m->array_max;
    size_t memsize = m->elem_size * m->array_max;
    char *mem = malloc(memsize + pairsize);
    m->pair = (struct ep_keypair *) mem;
    m->array = &mem[pairsize];

    // init pair array to empty
    for (size_t i = 0; i < m->array_max; ++i) {
        struct ep_keypair *keypair = &m->pair[i];
        keypair->key = 0;
        keypair->index = -1;
    }
}


void ep_map_free(struct ep_map *m) {
    free(m->array);
}


int ep_map_id(struct ep_map *m, int index) {
    void *item = &m->array[index * m->elem_size];
    return m->idfn(item);
}


int ep_map_insert(struct ep_map *m, void *elem) {
    int key = m->idfn(elem);
    size_t array_pos = ep_int_hash(key) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct ep_keypair *keypair = &m->pair[index];
        if (keypair->index < 0) {

            // place at the end of the value array
            keypair->key = key;
            keypair->index = m->elem_count++;
            void *item = &m->array[keypair->index * m->elem_size];
            memcpy(item, elem, m->elem_size);
            return 1;
        }
    }
    return 0;
}


int ep_map_erase(struct ep_map *m, int key) {
    size_t array_pos = ep_int_hash(key) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct ep_keypair *keypair = &m->pair[index];

        // match the key, and ensure pair is in use
        if (key == keypair->key && keypair->index >= 0) {

            // remove item from value array
            size_t movesize = m->elem_size * (m->elem_count - keypair->index);
            size_t dest_index = m->elem_size * keypair->index;
            size_t src_index = dest_index + m->elem_size;
            memmove(&m->array[dest_index], &m->array[src_index], movesize);

            // indicate the pair is no longer used
            keypair->index = -1;
            --m->elem_count;
            return 1;
        }
    }
    return 0;
}


void *ep_map_get(struct ep_map *m, int key) {
    size_t array_pos = ep_int_hash(key) % m->array_max;
    for (size_t i = 0; i < m->array_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct ep_keypair *keypair = &m->pair[index];

        // match the key, and ensure pair is in use
        if (key == keypair->key && keypair->index >= 0) {
            void *item = &m->array[keypair->index * m->elem_size];
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
    new_sa.begin = m->value_count;
    new_sa.end = m->value_count;
    ep_map_insert(&m->keys, &new_sa);
}


struct ep_subarray *ep_multimap_get_key(struct ep_multimap *m, int key) {
    return (struct ep_subarray *) ep_map_get(&m->keys, key);
}


struct ep_subarray *ep_multimap_get_index(struct ep_multimap *m, int index) {
    return (struct ep_subarray *) &m->keys.array[sizeof(struct ep_subarray) * index];
}


int ep_multimap_insert(struct ep_multimap *m, int key, size_t count) {
    struct ep_subarray *arr = ep_map_get(&m->keys, key);
    if (arr == NULL) {
        // create and init key
        ep_multimap_create_key(m, key);
        arr = ep_map_get(&m->keys, key);
    }

    // move elements to make space
    size_t newsize = m->elem_size * count;
    size_t endindex = m->elem_size * arr->end;
    size_t movesize = m->elem_size * (m->value_count - arr->end);
    memmove(&m->values[endindex + newsize], &m->values[endindex], movesize);


    int newstart = arr->end;
    arr->end += count;
    m->value_count += count;


    // increment all other subarrays
    for (int i = 0; i < m->keys.elem_count; ++i) {
        struct ep_subarray *isa = ep_multimap_get_index(m, i);
        if (isa->begin >= arr->end) {
            isa->begin += count;
            isa->end += count;
        }
    }
    return newstart - arr->begin;
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


void *ep_multimap_get_value(struct ep_multimap *m, int index) {
    if (index < m->value_count) {
        return &m->values[index * m->elem_size];
    }
    else {
        return NULL;
    }
}

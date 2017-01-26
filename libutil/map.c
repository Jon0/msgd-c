#include <string.h>

#include "map.h"


void msgu_map_init(struct msgu_map *m, msgu_map_hash_t hash, msgu_map_cmp_t cmp, size_t ksize, size_t vsize) {
    m->elem_count = 0;
    m->array_max = 0;
    m->chain_max = 8;
    m->hash = hash;
    m->cmp = cmp;
    m->key_size = ksize;
    m->data_size = vsize;
    m->elem_size = ksize + vsize;
}


void msgu_map_alloc(struct msgu_map *m, size_t count) {

    // allocate memory for 2 arrays
    size_t pairsize = sizeof(struct msgu_key_pair) * count;
    size_t memsize = m->elem_size * count;
    char *mem = malloc(memsize + pairsize);
    m->pair = (struct msgu_key_pair *) mem;
    m->array = &mem[pairsize];

    // init all pairs to empty
    m->array_max = count;
    for (size_t i = 0; i < m->array_max; ++i) {
        struct msgu_key_pair *keypair = &m->pair[i];
        keypair->hash = 0;
        keypair->index = -1;
    }
}


void msgu_map_free(struct msgu_map *m) {
    free(m->pair);
}


size_t msgu_map_size(struct msgu_map *m) {
    return m->elem_count;
}


hash_t msgu_map_id(struct msgu_map *m, int index) {
    if (0 <= index && index < m->elem_count) {
        void *item = &m->array[index * m->elem_size];
        return m->hash(item);
    }
    else {
        return 0;
    }
}


int msgu_map_insert(struct msgu_map *m, const void *key, void *elem) {
    hash_t hash = m->hash(key);
    size_t array_pos = hash % m->array_max;
    for (size_t i = 0; i < m->chain_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct msgu_key_pair *keypair = &m->pair[index];
        if (keypair->index < 0) {

            // place at the end of the value array
            keypair->hash = hash;
            keypair->index = m->elem_count++;
            char *item = &m->array[keypair->index * m->elem_size];
            memcpy(item, key, m->key_size);
            memcpy(&item[m->key_size], elem, m->data_size);
            return 1;
        }
    }
    return 0;
}


int msgu_map_erase(struct msgu_map *m, const void *key) {
    hash_t hash = m->hash(key);
    size_t array_pos = hash % m->array_max;
    for (size_t i = 0; i < m->chain_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct msgu_key_pair *keypair = &m->pair[index];

        // match the key, and ensure pair is in use
        if (hash == keypair->hash && keypair->index >= 0) {
            void *item = &m->array[keypair->index * m->elem_size];
            if (m->cmp(key, item) == 0) {

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
    }
    return 0;
}


void *msgu_map_get(struct msgu_map *m, const void *key) {
    hash_t hash = m->hash(key);
    size_t array_pos = hash % m->array_max;
    for (size_t i = 0; i < m->chain_max; ++i) {
        size_t index = (array_pos + i) % m->array_max;
        struct msgu_key_pair *keypair = &m->pair[index];

        // match the key, and ensure pair is in use
        if (hash == keypair->hash && keypair->index >= 0) {
            char *item = &m->array[keypair->index * m->elem_size];
            if (m->cmp(key, item) == 0) {
                return &item[m->key_size];
            }
        }
    }
    return NULL;
}


void *msgu_map_get_key(struct msgu_map *m, size_t index) {
    if (index < m->elem_count) {
        return &m->array[index * m->elem_size];
    }
    else {
        return NULL;
    }
}


void *msgu_map_get_value(struct msgu_map *m, size_t index) {
    if (index < m->elem_count) {
        char *item = &m->array[index * m->elem_size];
        return &item[m->key_size];
    }
    else {
        return NULL;
    }
}


void msgu_multimap_init(struct msgu_multimap *m, size_t esize, size_t count) {
    msgu_map_init(&m->keys, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct msgu_subarray));
    msgu_map_alloc(&m->keys, count);
    m->value_count = 0;
    m->elem_size = esize;
    m->array_max = count;
    size_t value_mem = m->elem_size * m->array_max;
    m->values = malloc(value_mem);
    memset(m->values, 0, value_mem);
}


int msgu_multimap_size(struct msgu_multimap *m, int key) {
    struct msgu_subarray *arr = msgu_map_get(&m->keys, &key);
    if (arr == NULL) {
        return 0;
    }
    return arr->end - arr->begin;
}


void msgu_multimap_create_key(struct msgu_multimap *m, int key) {
    struct msgu_subarray new_sa;
    new_sa.begin = m->value_count;
    new_sa.end = m->value_count;
    msgu_map_insert(&m->keys, &key, &new_sa);
}


struct msgu_subarray *msgu_multimap_get_key(struct msgu_multimap *m, int key) {
    return (struct msgu_subarray *) msgu_map_get(&m->keys, &key);
}


struct msgu_subarray *msgu_multimap_get_index(struct msgu_multimap *m, int index) {
    return (struct msgu_subarray *) &m->keys.array[sizeof(struct msgu_subarray) * index];
}


int msgu_multimap_insert(struct msgu_multimap *m, int key, size_t count) {
    struct msgu_subarray *arr = msgu_map_get(&m->keys, &key);
    if (arr == NULL) {
        // create and init key
        msgu_multimap_create_key(m, key);
        arr = msgu_map_get(&m->keys, &key);
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
        struct msgu_subarray *isa = msgu_multimap_get_index(m, i);
        if (isa->begin >= arr->end) {
            isa->begin += count;
            isa->end += count;
        }
    }
    return newstart - arr->begin;
}


void *msgu_multimap_get(struct msgu_multimap *m, int key, int index) {
    struct msgu_subarray *arr = msgu_map_get(&m->keys, &key);
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


void *msgu_multimap_get_value(struct msgu_multimap *m, int index) {
    if (index < m->value_count) {
        return &m->values[index * m->elem_size];
    }
    else {
        return NULL;
    }
}

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
    return index;
}


void *msgu_datatable_get_one(struct msgu_datatable *t, size_t index, size_t item) {
    size_t char_index = t->rowsize * index;
    for (int i = 0; i < item; ++i) {
        char_index += t->sizes[i];
    }
    return &t->data[char_index];
}


int msgu_datatable_emplace(struct msgu_datatable *t, void **table) {
    size_t index = t->row_count++;
    msgu_datatable_get(t, table, index);
    return index;
}


int msgu_datatable_erase(struct msgu_datatable *t, size_t index) {
    return -1;
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
    m->data = malloc(sizeof(struct msgu_datamap_key) * bits);
    m->alloc = bits;
    for (size_t i = 0; i < m->alloc; ++i) {
        struct msgu_datamap_key *keypair = &m->data[i];
        keypair->hash = 0;
        keypair->index = -1;
    }
}


int msgu_datamap_index(struct msgu_datamap *m, const void *key, void **row) {
    hash_t hash = m->hash(key);
    size_t array_pos = hash % m->alloc;
    for (size_t i = 0; i < m->chain; ++i) {
        size_t index = (array_pos + i) % m->alloc;
        struct msgu_datamap_key *keypair = &m->data[index];

        // match the key, and ensure pair is in use
        if (hash == keypair->hash && keypair->index >= 0) {
            char *item = msgu_datatable_get_one(m->table, keypair->index, m->item);
            if (m->cmp(key, item) == 0) {
                return msgu_datatable_get(m->table, row, keypair->index);
            }
        }
    }
    return -1;
}


int msgu_datamap_create_key(struct msgu_datamap *m, int index) {
    char *key = msgu_datatable_get_one(m->table, index, m->item);
    hash_t hash = m->hash(key);
    size_t array_pos = hash % m->alloc;
    for (size_t i = 0; i < m->chain; ++i) {
        size_t hash_index = (array_pos + i) % m->alloc;
        struct msgu_datamap_key *keypair = &m->data[hash_index];
        if (keypair->index < 0) {
            keypair->hash = hash;
            keypair->index = index;
            return index;
        }
    }
    return -1;
}

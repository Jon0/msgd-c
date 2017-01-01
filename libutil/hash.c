#include <stdint.h>

#include "hash.h"


hash_t msgu_int_hash(void *p) {
    return msgu_fast_hash(p, sizeof(int));
}


int msgu_int_cmp(void *a, void *b) {
    int *ia = a;
    int *ib = b;
    return (*ia < *ib) ? -1 : (*ia > *ib);
}


hash_t msgu_uint32_hash(void *p) {
    return msgu_fast_hash(p, sizeof(uint32_t));
}


int msgu_uint32_cmp(void *a, void *b) {
    uint32_t *ia = a;
    uint32_t *ib = b;
    return (*ia < *ib) ? -1 : (*ia > *ib);
}


hash_t msgu_fast_hash(void *data, size_t count) {
    const char *str = data;
    hash_t hash = 5381;
    int c;
    for (int i = 0; i < count; ++i) {
        c = *str++;
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

#include <stdint.h>

#include "hash.h"


hash_t msgu_int_hash(const void *p) {
    return msgu_fast_hash(p, sizeof(int));
}


int msgu_int_cmp(const void *a, const void *b) {
    const int *ia = a;
    const int *ib = b;
    return (*ia < *ib) ? -1 : (*ia > *ib);
}


hash_t msgu_uint32_hash(const void *p) {
    return msgu_fast_hash(p, sizeof(uint32_t));
}


int msgu_uint32_cmp(const void *a, const void *b) {
    const uint32_t *ia = a;
    const uint32_t *ib = b;
    return (*ia < *ib) ? -1 : (*ia > *ib);
}


hash_t msgu_fast_hash(const void *data, size_t count) {
    const char *str = data;
    hash_t hash = 5381;
    int c;
    for (int i = 0; i < count; ++i) {
        c = *str++;
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}


int msgu_binary_cmp(const char *a, const char *b, size_t a_len, size_t b_len) {
    int min = (a_len < b_len) ? a_len : b_len;
    int cmp = memcmp(a, b, min);
    if (cmp) {
        return cmp;
    }
    else {
        return a_len - b_len;
    }
}

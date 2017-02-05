#include "address.h"


hash_t msgu_address_hash(const void *p) {
    const struct msgu_address *addr = p;
    return msgu_fast_hash(addr->data, addr->len);
}


int msgu_address_cmp(const void *a, const void *b) {
    const struct msgu_address *addr_a = a;
    const struct msgu_address *addr_b = b;
    return msgu_binary_cmp(addr_a->data, addr_b->data, addr_a->len, addr_b->len);
}

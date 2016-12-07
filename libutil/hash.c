#include "hash.h"


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

#ifndef LIBUTIL_HASH_H
#define LIBUTIL_HASH_H

#include <stdlib.h>

typedef unsigned long hash_t;

hash_t msgu_fast_hash(void *data, size_t count);

#endif

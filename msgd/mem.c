#include <string.h>

#include "mem.h"


void init_mem_block(struct mem_block *b, size_t size) {
    b->ptr = malloc(size);
    b->used_bytes = 0;
    b->available_bytes = size;
}


void free_mem_block(struct mem_block *b) {
    if (b->ptr) {
        free(b->ptr);
    }
    b->used_bytes = 0;
    b->available_bytes = 0;
}


void resize_mem_block(struct mem_block *b, size_t size) {
    if (size < b->used_bytes) {
        // error
        return;
    }

    // allocate and copy to new block
    void *new_block = malloc(size);
    if (b->ptr) {
        memcpy(new_block, b->ptr, b->used_bytes);
        free(b->ptr);
    }
    b->ptr = new_block;
    b->available_bytes = size;
}

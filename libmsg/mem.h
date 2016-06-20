#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdint.h>


struct mem_block {
    void *ptr;
    size_t used_bytes;
    size_t available_bytes;
};


void init_mem_block(struct mem_block *b, size_t size);
void free_mem_block(struct mem_block *b);
void resize_mem_block(struct mem_block *b, size_t size);


#endif

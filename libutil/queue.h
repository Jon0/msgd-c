#ifndef LIBUTIL_QUEUE_H
#define LIBUTIL_QUEUE_H

#include "array.h"


struct msgu_queue {
    struct msgu_array arr;
    size_t begin;
    size_t size;
};


void msgu_queue_init(struct msgu_queue *q, size_t elem_size);
void msgu_queue_alloc(struct msgu_queue *q, size_t max);


int msgu_queue_pop(struct msgu_queue *q, void *e, size_t count);
int msgu_queue_push(struct msgu_queue *q, void *e, size_t count);


#endif

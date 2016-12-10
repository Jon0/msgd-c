#include <string.h>

#include "queue.h"


void msgu_queue_init(struct msgu_queue *q, size_t elem_size) {
    msgu_array_init(&q->arr, elem_size);
    q->begin = 0;
    q->size = 0;
}


void msgu_queue_alloc(struct msgu_queue *q, size_t max) {
    msgu_array_alloc(&q->arr, max);
}


int msgu_queue_pop(struct msgu_queue *q, void *e, size_t count) {
    if (count > q->size) {
        return 0;
    }
    msgu_array_get_wrap(&q->arr, q->begin, e, count);
    q->begin = (q->begin + count) % q->arr.allocated;
    q->size -= count;
}


int msgu_queue_push(struct msgu_queue *q, void *e, size_t count) {
    if (count > q->size) {
        return 0;
    }
    size_t end = (q->begin + q->size) % q->arr.allocated;
    msgu_array_set_wrap(&q->arr, end, e, count);
    q->size += count;
}

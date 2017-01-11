#include <string.h>

#include "queue.h"


void msgu_queue_init(struct msgu_queue *q, const struct msgu_element *fns, size_t elem_size) {
    msgu_array_init(&q->arr, fns, elem_size);
    q->begin = 0;
    q->size = 0;
}


void msgu_queue_alloc(struct msgu_queue *q, size_t max) {
    msgu_array_alloc(&q->arr, max);
}


size_t msgu_queue_size(struct msgu_queue *q) {
    return q->size;
}


size_t msgu_queue_element_size(struct msgu_queue *q) {
    return q->arr.esize;
}


size_t msgu_queue_pop(struct msgu_queue *q, void *e, size_t count) {
    if (count > q->size) {
        return 0;
    }
    size_t get_count = msgu_array_get_wrap(&q->arr, q->begin, e, count);
    q->begin = (q->begin + get_count) % q->arr.allocated;
    q->size -= get_count;
    return get_count;
}


size_t msgu_queue_push(struct msgu_queue *q, void *e, size_t count) {
    if (count > q->size) {
        return 0;
    }
    size_t end = (q->begin + q->size) % q->arr.allocated;
    size_t set_count = msgu_array_set_wrap(&q->arr, end, e, count);
    q->size += set_count;
    return set_count;
}

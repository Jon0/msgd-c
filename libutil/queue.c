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


size_t msgu_queue_size(const struct msgu_queue *q) {
    return q->size;
}


size_t msgu_queue_element_size(const struct msgu_queue *q) {
    return q->arr.esize;
}


size_t msgu_queue_element_serial_size(const struct msgu_queue *q, size_t index) {
    size_t arr_index = (q->begin + index) % q->arr.allocated;
    return q->arr.fns->size(&q->arr.data[q->arr.esize * arr_index]);
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
    if (count > (q->arr.allocated - q->size)) {
        return 0;
    }
    size_t end = (q->begin + q->size) % q->arr.allocated;
    size_t set_count = msgu_array_set_wrap(&q->arr, end, e, count);
    q->size += set_count;
    return set_count;
}


size_t msgu_queue_frag_size(const void *q) {
    const struct msgu_queue *queue = q;
    return sizeof(queue->size) + msgu_array_serial_size(&queue->arr, queue->begin, queue->size);
}


int msgu_queue_frag_read(struct msgu_stream *src, struct msgu_fragment *f, void *q) {
    struct msgu_queue *queue = q;
    int result;
    if (f[0].index == 0) {
        result = msgu_read_fixed(src, &f[1], (void *) &queue->size, sizeof(queue->size));
        if (result == msgu_stream_complete) {
            queue->begin = 0;
            msgu_queue_alloc(queue, queue->size * 2);
            msgu_fragment_inc(&f[0]);
        }
    }
    if (f[0].index == 1) {
        result = msgu_array_read(src, &f[1], &queue->arr, queue->begin, queue->size);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&f[0]);
        }
    }
    return result;
}


int msgu_queue_frag_write(struct msgu_stream *dest, struct msgu_fragment *f, const void *q) {
    const struct msgu_queue *queue = q;
    int result;
    if (f[0].index == 0) {
        result = msgu_write_fixed(dest, &f[1], &queue->size, sizeof(queue->size));
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&f[0]);
        }
    }
    if (f[0].index == 1) {
        result = msgu_array_write(dest, &f[1], &queue->arr, queue->begin, queue->size);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&f[0]);
        }
    }
    return result;
}


hash_t msgu_queue_map_hash(const void *q) {
    return 0;
}


int msgu_queue_map_cmp(const void *a, const void *b) {
    return 0;
}

#include <stdlib.h>

#include "eventqueue.h"

void msgs_event_queue_init(struct msgs_event_queue *q, struct msgu_type *fns, size_t elem_size) {
    msgu_vector_init(&q->data, fns, elem_size);
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->empty, NULL);
}


void msgs_event_queue_alloc(struct msgs_event_queue *q, size_t max_queue) {
    msgu_vector_alloc(&q->data, max_queue);
}


void msgs_event_queue_free(struct msgs_event_queue *q) {

}


size_t msgs_event_queue_pop(struct msgs_event_queue *q, void *e, size_t count) {
    pthread_mutex_lock(&q->mutex);
    while (msgu_vector_size(&q->data) == 0) {
        pthread_cond_wait(&q->empty, &q->mutex);
    }
    msgu_vector_pop(&q->data, e, count);
    pthread_mutex_unlock(&q->mutex);
}


size_t msgs_event_queue_push(struct msgs_event_queue *q, void *e, size_t count) {
    pthread_mutex_lock(&q->mutex);
    msgu_vector_push(&q->data, e, count);
    pthread_cond_broadcast(&q->empty);
    pthread_mutex_unlock(&q->mutex);
}

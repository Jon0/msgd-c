#ifndef LIBSYS_EVENTMAP_H
#define LIBSYS_EVENTMAP_H

#include <pthread.h>

#include <libutil/vector.h>


/*
 * a queue with locks preventing ensuring thread safety
 * queue events cause socket data to get parsed into messages
 */
struct msgs_event_queue {
    struct msgu_vector data;
    pthread_cond_t    empty;
    pthread_mutex_t   mutex;
};


void msgs_event_queue_init(struct msgs_event_queue *q, struct msgu_type *fns, size_t elem_size);
void msgs_event_queue_alloc(struct msgs_event_queue *q, size_t max_queue);
void msgs_event_queue_free(struct msgs_event_queue *q);


/*
 * remove element from front of queue, copy to e
 * blocks until an event is recieved
 */
size_t msgs_event_queue_pop(struct msgs_event_queue *q, void *e, size_t count);


/*
 * add events to the back of the queue
 */
size_t msgs_event_queue_push(struct msgs_event_queue *q, void *e, size_t count);


#endif

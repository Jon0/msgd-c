#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#include "endpoint.h"
#include "table.h"


/*
 * events must handle:
 * handlers recieving updates
 * acceptors opening a new socket
 */
struct ep_event {
    int epid;
};


/*
 * threads handle the events
 */
struct ep_event_queue {
    pthread_cond_t   empty;
    pthread_mutex_t  mutex;
    struct ep_table *table;
    struct ep_event *event;
    size_t begin;
    size_t size;
    size_t avail;
    size_t active;
};


void ep_event_queue_init(struct ep_event_queue *q, struct ep_table *t, size_t max_queue);


/*
 * remove element from front of queue, copy to e
 * blocks until an event is recieved
 */
void ep_queue_pop(struct ep_event_queue *q, struct ep_event *e);


/*
 * add events to the back of the queue
 */
void ep_queue_push(struct ep_event_queue *q, struct ep_event *e);


void ep_queue_from_table(struct ep_event_queue *q);


/*
 * block until write completes
 */
ssize_t ep_write_buf(struct ep_event_queue *q, int epid, struct ep_buffer *b);
ssize_t ep_write_fd(struct ep_event_queue *q, int epid, int fd);


#endif

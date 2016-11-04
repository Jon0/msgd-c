#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#include "endpoint.h"
#include "table.h"


/*
 * should contain a stack of all visited epid
 */
struct ep_event {
    int event_id;
    int epid;
    int srcid;
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
    int next_id;
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


/*
 * enter main loop
 */
void ep_queue_from_table(struct ep_event_queue *q);
void ep_queue_table_event(struct ep_event_queue *q, int epid);


/*
 * use type of epid to decide how to handle
 * how will it add new events to the queue?
 * assume only one event returned per update for now
 */
void ep_queue_update(struct ep_event_queue *q, struct ep_event *ev);

/*
 * apply accept event
 */
int ep_queue_accept(struct ep_table *t, struct ep_acceptor *a);


/*
 * the sources have new input, apply required actions
 */
void ep_queue_read_ch(struct ep_event_queue *q, struct ep_event *ev, struct ep_channel *c);
void ep_queue_read_hdl(struct ep_event_queue *q, struct ep_event *ev, struct ep_handler *h);
void ep_queue_notify(struct ep_event_queue *q, struct ep_table_entry *e, int srcid);
size_t ep_queue_write(struct ep_event_queue *eq, int src, int dest, char *b, size_t count);

#endif

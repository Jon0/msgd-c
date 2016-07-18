#ifndef THREAD_H
#define THREAD_H

#include <sys/epoll.h>

#include "handler.h"


/*
 * events recieve bytes or accepted addr
 */
union event_attr {
    size_t count;
    struct ep_address *addr;
};


/*
 *
 */
struct ep_event {
    struct ep_handler *hdl;
    union event_attr attr;
};


/*
 * threads handle the events
 */
struct ep_event_queue {
    pthread_cond_t   empty;
    pthread_mutex_t  mutex;
    struct ep_event *event;
    size_t begin;
    size_t size;
    size_t avail;
    size_t active;
};


void ep_event_queue_init(struct ep_event_queue *q, size_t max_queue);


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
 * apply an event (called inside handler threads)
 */
void ep_apply_event(struct ep_event_queue *q, struct ep_event *e);


/*
 * callback for file descriptor events
 */
typedef void (*ep_recv_t)(struct ep_table *t, struct ep_handler *);


struct ep_hdl_id {
    int fd;
    ep_recv_t fn;
    struct ep_handler *hdl;
};


/*
 * listens for external events for a subset of handlers
 */
struct ep_loop_data {
    int epoll_fd;
    struct ep_table table;
    struct ep_hdl_id *fd_map;
    struct ep_event_queue *queue;
};


void ep_loop_init(struct ep_loop_data *d, struct ep_event_queue *q);

/*
 * add data source
 */
void ep_loop_source(struct ep_loop_data *d, struct ep_handler *h);


/*
 * wait for events
 */
void ep_loop_run(struct ep_loop_data *d);
void ep_loop_event(struct ep_loop_data *d, struct epoll_event *event);


/*
 * collection of handler threads
 */
struct ep_thread_pool {
    struct ep_event_queue queue;
    pthread_t *threads;
    size_t size;
};


/*
 * alloc memory for the pool
 */
void ep_thread_pool_create(struct ep_thread_pool *p, size_t threads);


/*
 * wait for all source threads to complete
 */
void ep_thread_pool_join(struct ep_thread_pool *p);


#endif

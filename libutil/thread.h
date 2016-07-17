#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <sys/epoll.h>

#include "buffer.h"


// endpoint is local, module or network
// external endpoints are file descriptors
enum ep_hdl_type {
    ep_src_internal,
    ep_src_external
};


/*
 * data visible inside each thread
 */
struct ep_event_view {
    struct ep_event  *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(struct ep_event_view *);


union ep_handler_id {
    int hid;
    struct ep_source *src;
};

/*
 * memory allocated per active input?
 */
struct ep_handler {
    union ep_handler_id src;
    ep_callback_t       callback;
    pthread_mutex_t     modify;
    struct ep_buffer    buf;
};


/*
 * recieves update notifications
 */
struct ep_handler_recv {
    union ep_handler_id  arr [8];
    size_t begin;
    size_t bytes;
};


/*
 *
 */
struct ep_event {
    struct ep_handler *hdl;
    size_t count;
};


/*
 * threads handle the events
 */
struct ep_event_queue {
    pthread_cond_t   empty;
    struct ep_event *etask;
    struct ep_event *itask;
    size_t intbegin;
    size_t extsize;
    size_t intsize;
};


/*
 * remove element from front of queue, copy to e
 * blocks until an event is recieved
 */
void ep_queue_pop(struct ep_event_queue *q, struct ep_event *e);


/*
 * add events to the back of the queue
 */
void ep_queue_ext(struct ep_event_queue *q, struct ep_event *e);
void ep_queue_int(struct ep_event_queue *q, struct ep_event *e);


/*
 * apply an event (called inside handler threads)
 */
void ep_apply_event(struct ep_event *e);


/*
 * produces events for the task queue
 * the notify fd recieves internal events
 */
struct ep_loop_data {
    int epoll_fd;
    int notify_fd;
    struct ep_event_queue *q;
};


void ep_loop_init(struct ep_loop_data *d);
void ep_loop(struct ep_loop_data *d);
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

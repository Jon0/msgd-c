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
struct ep_task_view {
    struct ep_handler  *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_event_t)(struct ep_task_view *);


union ep_task {
    int tbid;
    struct ep_source *src;
};

/*
 * memory allocated per active input?
 */
struct ep_handler {
    union ep_task      src;
    ep_event_t         fn;
    pthread_mutex_t    modify;
    pthread_cond_t     cond;
    struct ep_buffer   buf;
};



struct ep_task_recv {
    union ep_task arr [8];
    size_t begin;
    size_t bytes;
};


/*
 * threads can complete tasks in blocks
 */
struct ep_task_queue {
    struct ep_handler *etask;
    struct ep_handler *itask;
    size_t extsize;
    size_t intsize;
};


void ep_queue_ext(struct ep_task_queue *q);
void ep_queue_int(struct ep_task_queue *q);
void ep_queue_apply(struct ep_task_queue *q);


/*
 * produces events for the task queue
 * the notify fd recieves internal events
 */
struct ep_loop_data {
    int epoll_fd;
    int notify_fd;
    struct ep_task_queue *q;
};


void ep_loop_init(struct ep_loop_data *d);
void ep_loop(struct ep_loop_data *d);
void ep_loop_event(struct ep_loop_data *d, struct epoll_event *event);


/*
 * a thread to handle events
 * the src type determines how the thread waits for input
 */
struct ep_thread {
    int                    hdlid;
    int                    state;
    pthread_t              thread;
    struct ep_thread_pool *pool;
};


/*
 * collection of handler threads
 */
struct ep_thread_pool {
    struct ep_thread *hdls;
    size_t size;
    size_t avail;
};


void ep_thread_init(struct ep_thread_pool *p, struct ep_thread *h);


/*
 * write input to a handler thread
 */
void ep_handler_write(struct ep_thread *h, struct ep_buffer *b);


/*
 * alloc memory for the pool
 */
void ep_thread_pool_init(struct ep_thread_pool *p);
void ep_thread_pool_free(struct ep_thread_pool *p);


/*
 * initialise and start a thread
 */
int ep_thread_pool_start(struct ep_thread_pool *p, ep_event_t fn);
int ep_thread_pool_stop(struct ep_thread_pool *p, struct ep_thread *h);


/*
 * wait for all source threads to complete
 */
void ep_thread_pool_join(struct ep_thread_pool *p);


#endif

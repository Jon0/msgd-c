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
 * waits for input on file descriptor
 */
struct ep_hdl_poll {
    int epid;
};


/*
 * waits for another thread to pass input
 */
struct ep_hdl_wait {
    pthread_cond_t cond;
};


/*
 * handler condition
 */
union ep_hdl_cond {
    struct ep_hdl_poll poll;
    struct ep_hdl_wait wait;
};


/*
 * data visible inside each thread
 */
struct ep_thread_view {
    struct ep_thread_pool *pool;
    struct ep_thread      *self;
};


/*
 * function to run as a new thread
 */
typedef void (*handler_main_t)(struct ep_thread_view *);


/*
 * memory allocated per active input?
 */
struct ep_task_ext {
    enum ep_hdl_type       htype;
    union ep_hdl_cond      hcond;
    handler_main_t         fn;
    pthread_mutex_t        modify;
    struct ep_buffer       buf;
};


struct ep_task_int {
    int id;
    size_t count;
};


/*
 * threads can complete tasks in blocks
 */
struct ep_task_queue {
    union ep_hdl_cond *taskmap; // sorts tasks by type
    size_t avail_task;
    struct ep_task_ext *etask;
    struct ep_task_int *itask;
    size_t extsize;
    size_t intsize;
};


void ep_queue_ext(struct ep_task_queue *q);
void ep_queue_int(struct ep_task_queue *q);

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
int ep_thread_pool_start(struct ep_thread_pool *p, handler_main_t fn);
int ep_thread_pool_stop(struct ep_thread_pool *p, struct ep_thread *h);


/*
 * wait for all source threads to complete
 */
void ep_thread_pool_join(struct ep_thread_pool *p);


#endif

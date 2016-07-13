#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include "buffer.h"


// endpoint is local, module or network
// external endpoints are file descriptors
enum ep_hdl_type {
    ep_src_internal,
    ep_src_external
};


/*
 * handler condition
 */
struct ep_hdl_cond {
    void *obj;
    void (*copy)(void *, size_t);
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
 * a thread to handle events
 * the src type determines how the thread waits for input
 */
struct ep_thread {
    int                    hdlid;
    int                    state;
    pthread_t              thread;
    enum ep_hdl_type       htype;
    struct ep_hdl_cond     hcond;
    pthread_mutex_t        modify;
    struct ep_buffer       buf;
    struct ep_thread_pool *pool;
    handler_main_t         fn;
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

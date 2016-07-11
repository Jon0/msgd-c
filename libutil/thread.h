#ifndef HANDLER_H
#define HANDLER_H

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
 * a thread to handle events
 * the src type determines how the thread waits for input
 */
struct ep_thread {
    int                hdlid;
    int                state;
    pthread_t          thread;
    enum ep_hdl_type   htype;
    struct ep_hdl_cond hcond;
    pthread_mutex_t    modify;
    struct ep_buffer   buf;
};


/*
 * collection of handler threads
 */
struct ep_thread_pool {
    struct ep_thread *hdls;
    size_t size;
};


struct ep_thread_view {
    struct ep_thread_pool *pool;
    struct ep_thread      *this_hdl;
};


/*
 * function to run as a new thread
 */
typedef void (*handler_main_t)(struct ep_thread_view *);


void ep_handler_init(struct ep_thread *h);

/*
 * initialise and start a thread
 */
int ep_handler_start(struct ep_thread_pool *p, handler_main_t fn);


/*
 * wait for all source threads to complete
 */
void ep_pool_join(struct ep_thread_pool *p);


/*
 * write input to a handler thread
 */
void ep_handler_write(struct ep_thread *h, struct ep_buffer *b);


#endif

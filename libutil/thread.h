#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include "queue.h"


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
void ep_thread_pool_create(struct ep_thread_pool *p, struct ep_table *tb, size_t threads);


/*
 * wait for all source threads to complete
 */
void ep_thread_pool_join(struct ep_thread_pool *p);


#endif

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include <libutil/queue.h>


/*
 * handles events with threads
 */
typedef void *(*msgs_thread_callback_t)(void *);


/*
 * collection of handler threads
 */
struct msgs_thread_pool {
    msgs_thread_callback_t callback;
    pthread_t             *threads;
    size_t                 thread_count;
};


/*
 * configure thread pool
 */
void msgs_thread_pool_init(struct msgs_thread_pool *pool, msgs_thread_callback_t cb);


/*
 * alloc memory for the pool and start threads
 */
void msgs_thread_pool_start(struct msgs_thread_pool *pool, size_t threads);


/*
 * wait for all source threads to complete
 */
void msgs_thread_pool_join(struct msgs_thread_pool *pool);


/*
 * mutex type
 */
typedef pthread_mutex_t msgs_mutex_t;
typedef void (*msgs_mutex_callback_t)(void *);


void msgs_mutex_init(msgs_mutex_t *mutex);
int msgs_mutex_try(msgs_mutex_t *mutex, msgs_mutex_callback_t callback, void *arg);
int msgs_mutex_lock(msgs_mutex_t *mutex, msgs_mutex_callback_t callback, void *arg);


/*
 * threads handle the events
 */
struct ep_event_queue {
    pthread_cond_t    empty;
    pthread_mutex_t   mutex;
    struct msgu_queue data;
};


void ep_event_queue_init(struct ep_event_queue *q, size_t elem_size);
void ep_event_queue_alloc(struct ep_event_queue *q, size_t max_queue);


/*
 * remove element from front of queue, copy to e
 * blocks until an event is recieved
 */
size_t ep_event_queue_pop(struct ep_event_queue *q, void *e, size_t count);


/*
 * add events to the back of the queue
 */
size_t ep_event_queue_push(struct ep_event_queue *q, void *e, size_t count);


#endif

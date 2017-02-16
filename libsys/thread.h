#ifndef LIBSYS_THREAD_H
#define LIBSYS_THREAD_H

#include <pthread.h>


/*
 * handles events with threads
 */
typedef void *(*msgs_thread_callback_t)(void *);


/*
 * collection of handler threads
 */
struct msgs_thread_pool {
    msgs_thread_callback_t callback;
    void                  *arg;
    pthread_t             *threads;
    size_t                 thread_count;
};


/*
 * configure thread pool
 */
void msgs_thread_pool_init(struct msgs_thread_pool *pool, msgs_thread_callback_t cb, void *arg);


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
void msgs_mutex_lock(msgs_mutex_t *mutex);
void msgs_mutex_unlock(msgs_mutex_t *mutex);
int msgs_mutex_try_lock(msgs_mutex_t *mutex);
int msgs_mutex_try(msgs_mutex_t *mutex, msgs_mutex_callback_t callback, void *arg);


/*
 * condition type
 */
typedef pthread_cond_t msgs_condition_t;


void msgs_condition_init(msgs_condition_t *cond);
void msgs_condition_wait(msgs_condition_t *cond, msgs_mutex_t *mutex);
void msgs_condition_signal(msgs_condition_t *cond);




#endif

#include <stdio.h>
#include <string.h>

#include "thread.h"


void msgs_thread_pool_init(struct msgs_thread_pool *pool, msgs_thread_callback_t cb, void *arg) {
    pool->callback = cb;
    pool->arg = arg;
    pool->threads = NULL;
    pool->thread_count = 0;
}


void msgs_thread_pool_start(struct msgs_thread_pool *pool, size_t threads) {
    size_t total_size = sizeof(pthread_t) * threads;
    pool->threads = malloc(total_size);
    memset(pool->threads, 0, total_size);
    pool->thread_count = threads;

    // create the threads
    for (int i = 0; i < threads; ++i) {
        int err = pthread_create(&pool->threads[i], NULL, pool->callback, pool->arg);
        if (err) {
            perror("pthread_create");
        }
    }
}


void msgs_thread_pool_join(struct msgs_thread_pool *pool) {
    for (int i = 0; i < pool->thread_count; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
    pool->thread_count = 0;
    free(pool->threads);
    printf("all threads joined\n");
}


void msgs_mutex_init(msgs_mutex_t *mutex) {
    pthread_mutex_init(mutex, NULL);
}


void msgs_mutex_lock(msgs_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
}


void msgs_mutex_unlock(msgs_mutex_t *mutex) {
    pthread_mutex_unlock(mutex);
}


int msgs_mutex_try_lock(msgs_mutex_t *mutex) {
    return (pthread_mutex_trylock(mutex) == 0);
}


int msgs_mutex_try(msgs_mutex_t *mutex, msgs_mutex_callback_t callback, void *arg) {
    // ensure only one thread enters the callback
    if (pthread_mutex_trylock(mutex)) {
        // lock failed
        return -1;
    }
    else {
        callback(arg);
        pthread_mutex_unlock(mutex);
        return 0;
    }
}


void ep_event_queue_init(struct ep_event_queue *q, struct msgu_type *fns, size_t elem_size) {
    msgu_vector_init(&q->data, fns, elem_size);
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->empty, NULL);
}


void ep_event_queue_alloc(struct ep_event_queue *q, size_t max_queue) {
    msgu_vector_alloc(&q->data, max_queue);
}


size_t ep_event_queue_pop(struct ep_event_queue *q, void *e, size_t count) {
    pthread_mutex_lock(&q->mutex);
    while (msgu_vector_size(&q->data) == 0) {
        pthread_cond_wait(&q->empty, &q->mutex);
    }
    msgu_vector_pop(&q->data, e, count);
    pthread_mutex_unlock(&q->mutex);
}


size_t ep_event_queue_push(struct ep_event_queue *q, void *e, size_t count) {
    pthread_mutex_lock(&q->mutex);
    msgu_vector_push(&q->data, e, count);
    pthread_cond_broadcast(&q->empty);
    pthread_mutex_unlock(&q->mutex);
}

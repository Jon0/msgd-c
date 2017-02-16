#include <stdlib.h>
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


void msgs_condition_init(msgs_condition_t *cond) {
    pthread_cond_init(cond, NULL);
}


void msgs_condition_wait(msgs_condition_t *cond, msgs_mutex_t *mutex) {
    pthread_cond_wait(cond, mutex);
}


void msgs_condition_signal(msgs_condition_t *cond) {
    pthread_cond_broadcast(cond);
}

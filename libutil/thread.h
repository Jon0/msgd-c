#ifndef THREAD_H
#define THREAD_H

#include <stdlib.h>
#include <pthread.h>


typedef void *(*thread_fn_t)(void *);


struct thread_task {
    thread_fn_t fn;
};


struct thread_queue {
    pthread_mutex_t empty;
    pthread_mutex_t modify;
    struct thread_task *task;
    size_t task_front;
    size_t task_count;
    size_t task_avail;
    int run;
};


struct thread_pool {
    pthread_t *task_mem;
    size_t thread_count;
    struct thread_queue queue;
};


void queue_init(struct thread_queue *q, size_t maxtask);
void queue_free(struct thread_queue *q);
void queue_stop(struct thread_queue *q);
void queue_ins(struct thread_queue *q, struct thread_task *t);

void pool_init(struct thread_pool *tp, size_t thread_count);
void pool_join(struct thread_pool *tp);


#endif

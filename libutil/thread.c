#include <stdio.h>

#include "thread.h"


void *thread_main(void *p) {
    struct thread_queue *q = (struct thread_queue *) p;
    while (q->run) {

        // block while queue no tasks
        pthread_mutex_lock(&q->empty);
        if (!q->run) {
            pthread_mutex_unlock(&q->empty);
            return NULL;
        }

        // copy front element
        pthread_mutex_lock(&q->modify);
        struct thread_task copy = q->task[q->task_front];
        q->task_front = (q->task_front + 1) % q->task_avail;
        --q->task_count;
        pthread_mutex_unlock(&q->modify);

        // allow next thread if more elements remain
        printf("queue length %d\n", q->task_count);
        if (q->task_count > 0) {
            pthread_mutex_unlock(&q->empty);
        }

        // run the task
        copy.fn(p);
    }
}


void queue_init(struct thread_queue *q, size_t maxtask) {
    q->task = malloc(sizeof(struct thread_task) * maxtask);
    q->task_avail = maxtask;
    q->task_front = 0;
    q->task_count = 0;
    q->run = 1;
    pthread_mutex_init(&q->empty, NULL);
    pthread_mutex_init(&q->modify, NULL);

    // lock the empty mutex
    pthread_mutex_lock(&q->empty);
}


void queue_free(struct thread_queue *q) {
    q->task_avail = 0;
    q->task_front = 0;
    q->task_count = 0;
    free(q->task);
}


void queue_stop(struct thread_queue *q) {
    q->run = 0;
}


void queue_ins(struct thread_queue *q, struct thread_task *t) {
    pthread_mutex_lock(&q->modify);
    size_t back = (q->task_front + q->task_count) % q->task_avail;
    q->task[back] = *t;
    ++q->task_count;
    pthread_mutex_unlock(&q->modify);

    // unlock empty mutex
    pthread_mutex_unlock(&q->empty);
}

void pool_init(struct thread_pool *tp, size_t thread_count) {
    queue_init(&tp->queue, 256);
    tp->task_mem = malloc(sizeof(pthread_t) * thread_count);
    tp->thread_count = thread_count;
    for (int i = 0; i < thread_count; ++i) {
        int err = pthread_create(&tp->task_mem[i], NULL, thread_main, (void *) &tp->queue);
        if (err) {
            perror("pthread_create");
            return;
        }
    }
}


void pool_join(struct thread_pool *tp) {
    for (int i = 0; i < tp->thread_count; ++i) {
        pthread_join(tp->task_mem[i], NULL);
    }
    free(tp->task_mem);
}

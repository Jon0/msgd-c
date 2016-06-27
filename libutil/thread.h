#ifndef THREAD_H
#define THREAD_H

#include <stdlib.h>
#include <pthread.h>


struct thread_pool {
    pthread_t *tmem;
    size_t tcount;
};


void thread_init(struct thread_pool *tp, size_t thread_count);

#endif

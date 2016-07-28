#include <stdio.h>
#include <string.h>

#include "thread.h"


void *ep_pthread_main(void *p) {
    struct ep_event_queue *q = (struct ep_event_queue *) p;
    while (1) {

        // wait for events from the queue
        struct ep_event ev;
        ep_queue_pop(q, &ev);
        ep_table_update(q->table, ev.epid);
    }
}


void ep_thread_pool_create(struct ep_thread_pool *p, struct ep_table *tb, size_t threads) {
    ep_event_queue_init(&p->queue, tb, 256);
    size_t total_size = sizeof(pthread_t) * threads;
    p->threads = malloc(total_size);
    memset(p->threads, 0, total_size);
    p->size = threads;

    // create the threads
    for (int i = 0; i < threads; ++i) {
        int err = pthread_create(&p->threads[i], NULL, ep_pthread_main, &p->queue);
        if (err) {
            perror("pthread_create");
        }
    }
}


void ep_thread_pool_join(struct ep_thread_pool *p) {
    for (int i = 0; i < p->size; ++i) {
        pthread_join(p->threads[i], NULL);
    }
    p->size = 0;
    free(p->threads);
    printf("all threads joined\n");
}

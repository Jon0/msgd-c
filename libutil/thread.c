#include <stdio.h>
#include <string.h>

#include "thread.h"


void *ep_pthread_main(void *p) {
    struct ep_event_queue *q = (struct ep_event_queue *) p;
    while (1) {

        // wait for events from the queue
        struct ep_event ev;
        ep_queue_pop(q, &ev);
        ep_apply_event(q, &ev);
    }
}


void ep_event_queue_init(struct ep_event_queue *q, struct ep_table *t, size_t max_queue) {
    q->table = t;
    q->event = malloc(sizeof(struct ep_event) * max_queue);
    q->begin = 0;
    q->size = 0;
    q->avail = max_queue;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->empty, NULL);
}


void ep_queue_pop(struct ep_event_queue *q, struct ep_event *e) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->empty, &q->mutex);
    }
    *e = q->event[q->begin];
    q->begin = (q->begin + 1) % q->avail;
    --q->size;
    pthread_mutex_unlock(&q->mutex);
}


void ep_queue_push(struct ep_event_queue *q, struct ep_event *e) {
    pthread_mutex_lock(&q->mutex);
    size_t i = (q->begin + q->size) % q->avail;
    struct ep_event *back = &q->event[i];
    *back = *e;
    ++q->size;
    pthread_cond_broadcast(&q->empty);
    pthread_mutex_unlock(&q->mutex);
}


void ep_apply_event(struct ep_event_queue *q, struct ep_event *e) {
    // if the epid has a source
    // then the input must be read
    if (e->recv) {
        e->recv(q->table, e->hdl->epid, &e->attr);
    }

    // check the callback conditions are met
    struct ep_event_view v;
    v.queue = q;
    v.self = e;
    e->hdl->callback(0, &v);
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

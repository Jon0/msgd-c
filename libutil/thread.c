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


void ep_event_queue_init(struct ep_event_queue *q, size_t max_queue) {
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
    // data has already been copied into buffer
    // check the callback conditions are met
    struct ep_event_view v;
    v.queue = q;
    v.self = e;
    e->hdl->callback(0, &v);
}


void ep_loop_init(struct ep_loop_data *d, struct ep_event_queue *q) {
    ep_table_init(&d->table);
    d->epoll_fd = epoll_create1(0);
    d->queue = q;
}


void ep_loop_source(struct ep_loop_data *d, struct ep_handler *h) {
    struct epoll_event ev;
    ev.events = EPOLLIN;

    struct ep_source *s = h->id.addr->src;
    int err = epoll_ctl(d->epoll_fd, EPOLL_CTL_ADD, s->fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}


void ep_loop_run(struct ep_loop_data *d) {
    struct epoll_event event [8];
    while (1) {
        int p = epoll_wait(d->epoll_fd, event, 8, 0);
        if (p == -1) {
            perror("epoll_wait");
        }
        else {
            for (int i = 0; i < p; ++i) {
                ep_loop_event(d, &event[i]);
            }
        }
    }
}


void ep_loop_event(struct ep_loop_data *d, struct epoll_event *event) {
    printf("epoll event recieved\n");
    struct ep_event e;
    e.hdl = d->fd_map[event->data.fd].hdl;
    ep_queue_push(d->queue, &e);
}


void ep_thread_pool_create(struct ep_thread_pool *p, size_t threads) {
    ep_event_queue_init(&p->queue, 256);
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

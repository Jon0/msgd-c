#include <stdio.h>
#include <string.h>

#include "thread.h"


void *ep_pthread_main(void *p) {
    struct ep_event_queue *q = (struct ep_event_queue *) p;
    while (1) {

        // wait for events from the queue
        struct ep_event ev;
        ep_queue_pop(q, &ev);
        ep_apply_event(&ev);

        return 0;
    }
}


void ep_queue_pop(struct ep_event_queue *q, struct ep_event *e) {
    *e = q->itask[q->intbegin];
}


void ep_queue_ext(struct ep_event_queue *q, struct ep_event *e) {

}


void ep_queue_int(struct ep_event_queue *q, struct ep_event *e) {

}


void ep_apply_event(struct ep_event *e) {
    // data has already been copied into buffer
    // check the callback conditions are met
    struct ep_event_view v;
    v.self = e;
    e->hdl->callback(&v);
}


void ep_loop_init(struct ep_loop_data *d) {
    d->epoll_fd = epoll_create1(0);
}


void ep_loop(struct ep_loop_data *d) {
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
    struct ep_event e;
    if (event->data.fd == d->notify_fd) {
        // read id of triggered handler
        struct ep_handler_recv i;

        ep_queue_int(d->q, &e);


    }
    else {
        // external event
        ep_queue_ext(d->q, &e);
    }
}


void ep_thread_pool_create(struct ep_thread_pool *p, size_t threads) {
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

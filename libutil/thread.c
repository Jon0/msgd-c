#include <stdio.h>
#include <string.h>

#include "thread.h"


void *ep_pthread_main(void *p) {

}


void ep_queue_ext(struct ep_task_queue *q) {

}


void ep_queue_int(struct ep_task_queue *q) {

}


void ep_queue_apply(struct ep_task_queue *q) {
    // apply the longest queue first
    for (int i = 0; i < q->intsize; ++i) {
        // data has already been copied into buffer
        struct ep_event_view v;
        v.self = &q->itask[i];
        v.self->callback(&v);
    }
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
    if (event->data.fd == d->notify_fd) {
        // read id of triggered handler
        struct ep_handler_recv i;

        ep_queue_int(d->q);


    }
    else {
        // external event
        ep_queue_ext(d->q);
    }
}


void ep_thread_init(struct ep_thread_pool *p, struct ep_thread *h) {
    h->pool = p;
}


void ep_thread_pool_init(struct ep_thread_pool *p) {
    size_t hcount = 256;
    size_t total_size = sizeof(struct ep_thread) * hcount;
    p->hdls = malloc(total_size);
    memset(p->hdls, 0, total_size);
    p->size = 0;
    p->avail = hcount;
}


void ep_thread_pool_free(struct ep_thread_pool *p) {
    p->size = 0;
    p->avail = 0;
    free(p->hdls);
}


int ep_thread_pool_start(struct ep_thread_pool *p, ep_event_t fn) {
    struct ep_thread *h = &p->hdls[p->size++];
    ep_thread_init(p, h);

    // try start the thread
    int err = pthread_create(&h->thread, NULL, ep_pthread_main, h);
    if (err) {
        perror("pthread_create");
    }
    else {
            h->state = 1;
    }
    return 0;
}


int ep_thread_pool_stop(struct ep_thread_pool *p, struct ep_thread *h) {
    // is the thread already joined?
    h->state = 0;
    --p->size;
    return 0;
}


void ep_thread_pool_join(struct ep_thread_pool *p) {
    size_t i = 0;
    while (p->size) {
        struct ep_thread *h = &p->hdls[i];
        if (h->state) {
            pthread_join(h->thread, NULL);
        }
        i = (i + 1) % p->avail;
    }
    printf("all threads joined\n");
}

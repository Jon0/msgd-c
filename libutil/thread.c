#include <stdio.h>
#include <string.h>

#include "thread.h"


/*
 * wrap the handler main with the pthread function type
 */
void *ep_pthread_start(void *p) {
    struct ep_thread *t = (struct ep_thread *) p;
    struct ep_thread_view v;
    v.pool = t->pool;
    v.self = t;
    t->fn(&v);
    ep_thread_pool_stop(v.pool, v.self);
    return NULL;
}


void ep_thread_init(struct ep_thread_pool *p, struct ep_thread *h) {
    h->pool = p;
    pthread_mutex_init(&h->modify, NULL);
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


int ep_thread_pool_start(struct ep_thread_pool *p, handler_main_t fn) {
    struct ep_thread *h = &p->hdls[p->size++];
    ep_thread_init(p, h);

    // collect data passed to thread
    h->fn = fn;
    int err = pthread_create(&h->thread, NULL, ep_pthread_start, h);
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

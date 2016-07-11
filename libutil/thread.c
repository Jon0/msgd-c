#include <stdio.h>

#include "thread.h"


struct ep_pthread_data {
    struct ep_thread_view st;
    handler_main_t fn;
};


/*
 * wrap the handler main with the pthread function type
 */
void *ep_pthread_start(void *p) {
    struct ep_pthread_data d = *(struct ep_pthread_data *) p;
    d.fn(&d.st);
    return NULL;
}


void ep_handler_init(struct ep_thread *h) {
    pthread_mutex_init(&h->modify, NULL);
}


int ep_handler_start(struct ep_thread_pool *p, handler_main_t fn) {
    struct ep_thread *h = &p->hdls[p->size];
    ep_handler_init(h);

    // collect data passed to thread
    struct ep_pthread_data thread_data;
    thread_data.st.pool = p;
    thread_data.st.this_hdl = h;
    thread_data.fn = fn;
    int err = pthread_create(&h->thread, NULL, ep_pthread_start, &thread_data);
    if (err) {
        perror("pthread_create");
    }
    else {
            h->state = 1;
    }
    return 0;
}


void ep_pool_join(struct ep_thread_pool *p) {
    // how many threads are running?
    for (int i = 0; i < p->size; ++i) {
        struct ep_thread *h = &p->hdls[i];
        if (h->state) {
            pthread_join(h->thread, NULL);
        }
    }
    printf("all threads joined\n");
}

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "poll.h"
#include "table.h"
#include "thread.h"


void *msgs_table_thread(void *t) {
    struct msgs_table *table = t;
    msgs_table_poll(table);
    return NULL;
}


void msgs_table_init(struct msgs_table *t, size_t max, struct msgs_handlers *h, void *arg) {
    pthread_mutex_init(&t->mutex, NULL);
    t->hdl = *h;
    t->arg = arg;

    // reserve id 0 for inotify events
    t->next_id = 1;
    t->epoll_fd = ep_poll_create();
    t->inotify_fd = ep_notify_create();
    ep_poll_enable(t->epoll_fd, 0, t->inotify_fd);
}


void msgs_table_free(struct msgs_table *t) {

}


void msgs_table_start(struct msgs_table *t, int threads) {
    struct msgs_thread_pool pool;
    msgs_thread_pool_init(&pool, msgs_table_thread);
    msgs_thread_pool_start(&pool, threads);
    msgs_thread_pool_join(&pool);
}


void msgs_table_poll(struct msgs_table *t) {
    struct epoll_event event [32];
    while (1) {
        int p = epoll_wait(t->epoll_fd, event, 32, -1);
        if (p == -1) {
            perror("epoll_wait");
            return;
        }
        else {
            for (int i = 0; i < p; ++i) {
                msgs_table_recv(t, event[i].events, event[i].data.u64);
            }
        }
    }
}


void msgs_table_recv(struct msgs_table *t, msgs_event_t e, msgs_poll_t id) {

    // bitshift id to find type quickly

    // TODO lookup event type
    //t->callback(t->arg, NULL);
}


int msgs_open_acceptor(struct msgs_table *t, struct msgs_acceptor *a) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    ep_poll_enable(t->epoll_fd, epid, a->fd);
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_open_socket(struct msgs_table *t, struct msgs_socket *s) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    ep_poll_enable(t->epoll_fd, epid, s->fd);
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_open_file(struct msgs_table *t, struct msgs_file *f) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_close(struct msgs_table *t, int epid) {
    pthread_mutex_lock(&t->mutex);
    // close fd, and epoll
    // remove from map

    pthread_mutex_unlock(&t->mutex);
    return 0;
}


int ep_table_notify_read(struct msgs_table *t) {
    int wd = ep_notify_read(t->inotify_fd);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

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
    t->epoll_fd = epoll_create1(0);
    t->inotify_fd = inotify_init1(IN_NONBLOCK);
    msgs_table_enable(t->epoll_fd, t->inotify_fd, 4, 0);
}


void msgs_table_free(struct msgs_table *t) {
    close(t->inotify_fd);
}


void msgs_table_enable(int epfd, int fd, uint32_t type, uint32_t id) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u64 = (((uint64_t) type) << 32) | id;

    // add to epoll
    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}


void msgs_table_start(struct msgs_table *t, int threads) {
    struct msgs_thread_pool pool;
    msgs_thread_pool_init(&pool, msgs_table_thread, t);
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
                uint32_t type = (uint32_t) (event[i].data.u64 >> 32);
                uint32_t id = (uint32_t) event[i].data.u64;
                msgs_table_recv(t, event[i].events, type, id);
            }
        }
    }
}


void msgs_table_recv(struct msgs_table *t, uint32_t e, uint32_t type, uint32_t id) {
    printf("table recv: %u, %u, %u\n", e, type, id);

    // TODO lookup event type
    //t->callback(t->arg, NULL);
}


int msgs_add_acceptor(struct msgs_table *t, struct msgs_acceptor *a) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    msgs_table_enable(t->epoll_fd, a->fd, 5, epid);
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_add_socket(struct msgs_table *t, struct msgs_socket *s) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    msgs_table_enable(t->epoll_fd, s->fd, 6, epid);
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_add_file(struct msgs_table *t, struct msgs_file *f) {
    pthread_mutex_lock(&t->mutex);
    int epid = t->next_id++;
    pthread_mutex_unlock(&t->mutex);
    return epid;
}


int msgs_remove(struct msgs_table *t, int epid) {
    pthread_mutex_lock(&t->mutex);
    // remove from epoll

    pthread_mutex_unlock(&t->mutex);
    return 0;
}


int ep_table_notify_read(struct msgs_table *t) {
    int wd = ep_notify_read(t->inotify_fd);
    return 0;
}

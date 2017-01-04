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


void msgs_table_init(struct msgs_table *t, struct msgu_event_map *map) {
    t->epoll_fd = epoll_create1(0);
    t->inotify_fd = inotify_init1(IN_NONBLOCK);
    t->emap = map;
    pthread_mutex_init(&t->ctl_mutex, NULL);
    pthread_mutex_init(&t->map_mutex, NULL);
    msgs_table_enable(t, t->inotify_fd, 4, 0);
}


void msgs_table_free(struct msgs_table *t) {
    close(t->inotify_fd);
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


void msgs_table_recv(struct msgs_table *t, uint32_t ev, uint32_t type, uint32_t id) {
    union msgu_any_event data;
    pthread_mutex_lock(&t->map_mutex);
    int count = msgu_event_copy(t->emap, id, &data);
    pthread_mutex_unlock(&t->map_mutex);
    if (count) {
        msgu_event_notify(t->emap, type, &data);
    }
    else {
        printf("event %d, %d not found\n", type, id);
    }
}


int ep_table_notify_read(struct msgs_table *t) {
    int wd = ep_notify_read(t->inotify_fd);
    return 0;
}


void msgs_table_enable(struct msgs_table *t, int fd, uint32_t type, uint32_t id) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u64 = (((uint64_t) type) << 32) | id;

    // add to epoll
    pthread_mutex_lock(&t->ctl_mutex);
    int err = epoll_ctl(t->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
    pthread_mutex_unlock(&t->ctl_mutex);
}


int msgs_poll_acceptor(struct msgs_table *t, struct msgs_acceptor *acc) {
    struct msgu_connect_event ce;
    pthread_mutex_lock(&t->map_mutex);
    int id = msgu_add_conn(t->emap, &ce);
    pthread_mutex_unlock(&t->map_mutex);
    msgs_table_enable(t, acc->fd, msgu_connect_id, id);
    return id;
}


int msgs_poll_socket(struct msgs_table *t, struct msgs_socket *sk) {

    // fill event using socket
    struct msgu_recv_event re;
    pthread_mutex_lock(&t->map_mutex);
    int id = msgu_add_recv(t->emap, &re);
    pthread_mutex_unlock(&t->map_mutex);
    msgs_table_enable(t, sk->fd, msgu_recv_id, id);
    return id;
}

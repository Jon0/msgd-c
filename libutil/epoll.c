#include <stdio.h>

#include "epoll.h"


void ep_loop_init(struct ep_loop_data *d, struct ep_event_queue *q) {
    ep_table_init(&d->table);
    d->fd_map = malloc(sizeof(struct ep_hdl_link) * 32);
    d->epoll_fd = epoll_create1(0);
    d->queue = q;
}


void ep_loop_source(struct ep_loop_data *d, struct ep_handler *h, ep_recv_t r) {
    struct epoll_event ev;
    ev.events = EPOLLIN;

    // add to map
    struct ep_source *s = ep_table_src(&d->table, h->epid);
    struct ep_hdl_link *l = &d->fd_map[s->fd];
    l->fd = s->fd;
    l->ev.hdl = h;
    l->ev.recv = r;

    // add to epoll
    int err = epoll_ctl(d->epoll_fd, EPOLL_CTL_ADD, s->fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}


void ep_loop_run(struct ep_loop_data *d) {
    struct epoll_event event [32];
    while (1) {
        printf("wait for events...\n");
        int p = epoll_wait(d->epoll_fd, event, 32, -1);
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
    struct ep_hdl_link *l = &d->fd_map[event->data.fd];
    ep_queue_push(d->queue, &l->ev);
}

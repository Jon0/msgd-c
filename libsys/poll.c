#include <stdio.h>
#include <sys/epoll.h>

#include "poll.h"


int ep_poll_create() {
    return epoll_create1(0);
}


int ep_poll_wait(int epfd, void *obj, ep_poll_callback_t cb) {
    struct epoll_event event [32];
    int p = epoll_wait(epfd, event, 32, -1);
    if (p == -1) {
        perror("epoll_wait");
        return -1;
    }
    else {
        for (int i = 0; i < p; ++i) {
            cb(obj, event[i].data.u32);
        }
    }
    return 0;
}


void ep_poll_enable(int epfd, int epid, int fd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u32 = epid;

    // add to epoll
    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}

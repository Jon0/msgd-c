#include <stdio.h>
#include <sys/epoll.h>

#include "poll.h"


int ep_poll_create() {
    return epoll_create1(0);
}


int ep_poll_wait(int epfd, int *src, size_t count) {
    struct epoll_event event [32];
    if (count > 32) {
        count = 32;
    }
    int p = epoll_wait(epfd, event, count, -1);
    if (p == -1) {
        perror("epoll_wait");
    }
    else {
        for (int i = 0; i < p; ++i) {
            src[i] = event[i].data.u32;
        }
        return p;
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

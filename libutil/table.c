#include "table.h"


size_t ep_entry_hash(void *p) {
    struct ep_table_entry *e = (struct ep_table_entry *) p;
    return e->epid;
}

void ep_table_init(struct ep_table *t) {
    t->epoll_fd = epoll_create1(0);
}


void ep_table_free(struct ep_table *t) {

}


int ep_table_wait(struct ep_table *t, struct ep_source **src, size_t count) {
    struct epoll_event event [32];
    printf("wait for events\n");
    int p = epoll_wait(t->epoll_fd, event, 32, -1);
    if (p == -1) {
        perror("epoll_wait");
    }
    else {
        for (int i = 0; i < p; ++i) {
            printf("id %d event\n", event[i].data.u32);
            src[i] = ep_table_src(t, event[i].data.u32);
        }
        return p;
    }
    return 0;
}


void ep_enable_src(struct ep_table *t, struct ep_source *s) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u32 = s->epid;

    // add to epoll
    int err = epoll_ctl(t->epoll_fd, EPOLL_CTL_ADD, s->fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}

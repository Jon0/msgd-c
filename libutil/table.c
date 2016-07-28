#include <stdio.h>

#include "table.h"


int ep_entry_id(void *p) {
    struct ep_table_entry *e = (struct ep_table_entry *) p;
    return e->epid;
}


void ep_table_init(struct ep_table *t, size_t max) {
    size_t es = sizeof(struct ep_table_entry);
    ep_map_alloc(&t->entries, ep_entry_id, es, max);
    t->epoll_fd = epoll_create1(0);
}


void ep_table_free(struct ep_table *t) {
    ep_map_free(&t->entries);
}


int ep_add_acceptor(struct ep_table *t, struct ep_acceptor *a) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_acceptor;
    e.data.acc = *a;
    ep_enable_fd(t, e.epid, a->fd);
    return e.epid;
}


int ep_add_channel(struct ep_table *t, struct ep_channel *c) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_channel;
    e.data.ch = *c;
    ep_enable_fd(t, e.epid, c->fd);
    return e.epid;
}


int ep_add_handler(struct ep_table *t, struct ep_handler *a) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_handler;
    return e.epid;
}


void ep_close(struct ep_table *t, int epid) {

}


void ep_table_ctl(struct ep_table *t, int epid) {

}


int ep_table_accept(struct ep_table *t, struct ep_acceptor *a) {
    struct ep_channel newc;
    newc.addr.len = 32;
    newc.fd = accept(a->fd, (struct sockaddr *) &newc.addr.data, &newc.addr.len);
    if (newc.fd < 0) {
        perror("accept");
        return 0;
    }
    return ep_add_channel(t, &newc);
}


int ep_table_wait(struct ep_table *t, int *src, size_t count) {
    struct epoll_event event [32];
    printf("wait for events\n");
    int p = epoll_wait(t->epoll_fd, event, 32, -1);
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


void ep_enable_fd(struct ep_table *t, int epid, int fd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u32 = epid;

    // add to epoll
    int err = epoll_ctl(t->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (err == -1) {
        perror("epoll_ctl");
    }
}


void ep_table_update(struct ep_table *t, int epid) {
    struct ep_table_entry *e = ep_map_get(&t->entries, epid);
    switch(e->type) {

    }
}

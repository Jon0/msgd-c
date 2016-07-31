#include <stdio.h>

#include "table.h"


int ep_entry_id(void *p) {
    struct ep_table_entry *e = (struct ep_table_entry *) p;
    return e->epid;
}


void ep_table_init(struct ep_table *t, size_t max) {
    t->next_id = 1;
    t->epoll_fd = epoll_create1(0);
    size_t es = sizeof(struct ep_table_entry);
    printf("alloc table (%d bytes)\n", es * max);
    ep_map_alloc(&t->entries, ep_entry_id, es, max);
}


void ep_table_free(struct ep_table *t) {
    ep_map_free(&t->entries);
}


int ep_add_acceptor(struct ep_table *t, struct ep_acceptor *a) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_acceptor;
    e.data.acc = *a;
    ep_map_insert(&t->entries, &e);
    ep_enable_fd(t, e.epid, a->fd);
    return e.epid;
}


int ep_add_channel(struct ep_table *t, struct ep_channel *c) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_channel;
    e.data.ch = *c;
    ep_map_insert(&t->entries, &e);
    ep_enable_fd(t, e.epid, c->fd);
    return e.epid;
}


int ep_add_handler(struct ep_table *t, struct ep_handler *h) {
    struct ep_table_entry e;
    e.epid = t->next_id++;
    e.type = ep_type_handler;
    e.data.hdl = *h;
    ep_map_insert(&t->entries, &e);
    return e.epid;
}


void ep_close(struct ep_table *t, int epid) {
    // close fd, and epoll

    // remove from map
    ep_map_erase(&t->entries, epid);
}


void ep_table_ctl(struct ep_table *t, int in, int out) {
    struct ep_table_entry *e = ep_map_get(&t->entries, in);
    if (e->type == ep_type_channel) {
        int ind = e->data.ch.outcount++;
        e->data.ch.output[ind] = out;
    }
}


int ep_table_addr(struct ep_table *t, int epid, struct ep_address *out) {
    struct ep_table_entry *e = ep_map_get(&t->entries, epid);
    if (e) {
        switch(e->type) {
        case ep_type_acceptor:
            *out = e->data.acc.addr;
            return 1;
        case ep_type_channel:
            *out = e->data.ch.addr;
            return 1;
        }
    }
    else {
        printf("%d not found\n", epid);
    }
    return 0;
}


int ep_table_wait(struct ep_table *t, int *src, size_t count) {
    struct epoll_event event [32];
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

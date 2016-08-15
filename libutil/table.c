#include <stdio.h>
#include <unistd.h>

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
    if (count > 32) {
        count = 32;
    }
    int p = epoll_wait(t->epoll_fd, event, count, -1);
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


size_t ep_table_read(struct ep_table *t, int epid, char *buf, size_t count) {
    struct ep_table_entry *e = ep_map_get(&t->entries, epid);
    int wr;
    if (e) {
        switch(e->type) {
        case ep_type_channel:
            wr = read(e->data.ch.fd, buf, count);
            if (wr < 0) {
                perror("read");
                return 0;
            }
            return wr;
        }
    }
    else {
        printf("%d not found\n", epid);
    }
    return 0;
}


size_t ep_table_read_buf(struct ep_table *t, int epid, struct ep_buffer *b) {
    struct ep_table_entry *e = ep_map_get(&t->entries, epid);
    int wr;
    if (e) {
        switch(e->type) {
        case ep_type_channel:
            wr = ep_buffer_take(b, e->data.ch.fd);
            if (wr < 0) {
                perror("read");
                return 0;
            }
            return wr;
        }
    }
    else {
        printf("%d not found\n", epid);
    }
    return 0;
}


void ep_table_fwd(struct ep_table *t, int epid) {
    struct ep_table_entry *e = ep_map_get(&t->entries, epid);
    if (e) {
        switch(e->type) {
        case ep_type_channel:
            return ep_channel_fwd(t, &e->data.ch);
        }
    }
    else {
        printf("%d not found\n", epid);
    }
}


void ep_channel_fwd(struct ep_table *t, struct ep_channel *c) {
    char buf [1024];
    int r = read(c->fd, buf, 1024);
    if (r > 0) {
        for (int i = 0; i < c->outcount; ++i) {
            struct ep_table_entry *e = ep_map_get(&t->entries, c->output[i]);
            if (e) {
                ep_fwd_blk(e, buf, r);
            }
        }
    }
}


size_t ep_fwd_buf(struct ep_table_entry *e, struct ep_buffer *b, size_t start) {
    switch(e->type) {
    case ep_type_channel:
        return ep_buffer_write(b, e->data.ch.fd, start);
    case ep_type_handler:
        return ep_buffer_copy(&e->data.hdl.buf, b, start);
    }
}


size_t ep_fwd_blk(struct ep_table_entry *e, char *b, size_t count) {
    switch(e->type) {
    case ep_type_channel:
        return ep_ch_write_blk(&e->data.ch, b, count);
    case ep_type_handler:
        return ep_hdl_write_blk(&e->data.hdl, b, count);
    }
}

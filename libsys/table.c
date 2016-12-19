#include <stdio.h>
#include <unistd.h>

#include "poll.h"
#include "table.h"


int msgs_table_recv_event(void *t, int id) {
    struct ep_table *table = t;

    // TODO lookup event type
    table->callback(table->callback_arg, NULL);
    return 0;
}


void ep_table_init(struct ep_table *t, size_t max, void *arg, msgs_table_event_t cb) {
    t->callback = cb;
    t->callback_arg = arg;

    // reserve id 0 for inotify events
    t->next_id = 1;
    t->epoll_fd = ep_poll_create();
    t->inotify_fd = ep_notify_create();
    ep_poll_enable(t->epoll_fd, 0, t->inotify_fd);

    // allocate table memory
    msgu_map_init(&t->entries, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct ep_table_entry));
    msgu_map_init(&t->watched, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct ep_table_watch));
    msgu_map_alloc(&t->entries, max);
    msgu_map_alloc(&t->watched, max);
    msgu_multimap_init(&t->accepted, sizeof(int), max);
    msgu_multimap_init(&t->chanout, sizeof(int), max);
}


void ep_table_free(struct ep_table *t) {
    msgu_map_free(&t->entries);
    msgu_map_free(&t->watched);
}


void msgs_table_queue_events(struct ep_table *t) {
    while (1) {
        int r = ep_poll_wait(t->epoll_fd, t, msgs_table_recv_event);
        if (r < 0) {
            printf("poll error\n");
            return;
        }
    }
}


int ep_add_acceptor(struct ep_table *t, struct ep_acceptor *a) {
    int epid = t->next_id++;
    struct ep_table_entry e;
    e.type = ep_type_acceptor;
    e.data.acc = *a;
    msgu_map_insert(&t->entries, &epid, &e);
    msgu_multimap_create_key(&t->accepted, epid);
    ep_poll_enable(t->epoll_fd, epid, a->fd);
    return epid;
}


int ep_add_channel(struct ep_table *t, struct ep_channel *c) {
    int epid = t->next_id++;
    struct ep_table_entry e;
    e.type = ep_type_channel;
    e.data.ch = *c;
    msgu_map_insert(&t->entries, &epid, &e);
    msgu_multimap_create_key(&t->chanout, epid);
    ep_poll_enable(t->epoll_fd, epid, c->fd);
    return epid;
}


int ep_add_handler(struct ep_table *t, struct ep_handler *h) {
    int epid = t->next_id++;
    struct ep_table_entry e;
    e.type = ep_type_handler;
    e.data.hdl = *h;
    msgu_map_insert(&t->entries, &epid, &e);
    return epid;
}


int ep_add_notify(struct ep_table *t, struct ep_notify *n) {
    int epid = t->next_id++;
    struct ep_table_entry e;
    struct ep_table_watch w;
    e.type = ep_type_notify;
    e.data.nf = *n;
    w.epid = epid;
    msgu_map_insert(&t->entries, &epid, &e);
    msgu_map_insert(&t->watched, &n->wd, &w);
    return epid;
}


void ep_close(struct ep_table *t, int epid) {
    // close fd, and epoll

    // remove from map
    msgu_map_erase(&t->entries, &epid);
}


void ep_table_print_id(struct ep_table *t, int epid) {
    struct ep_table_entry *e = msgu_map_get(&t->entries, &epid);
    if (e) {
        switch(e->type) {
        case ep_type_acceptor:
            printf("[%d] Acceptor ", epid);
            ep_address_print(&e->data.acc.addr);
            printf("\n");
            break;
        case ep_type_channel:
            printf("[%d] Channel ", epid);
            ep_address_print(&e->data.acc.addr);
            printf("\n");
            break;
        case ep_type_handler:
            printf("[%d] Handler\n", epid);
            ep_address_print(&e->data.acc.addr);
            printf("\n");
            break;
        }
    }
    else {
        printf("[%d] Not found\n", epid);
    }
}


void ep_table_route(struct ep_table *t, int in, int out) {
    struct ep_table_entry *e = msgu_map_get(&t->entries, &in);
    if (e->type == ep_type_channel) {
        int index = msgu_multimap_insert(&t->chanout, in, 1);
        int *chan = msgu_multimap_get(&t->chanout, in, index);
        *chan = out;
    }
}


int ep_table_addr(struct ep_table *t, int epid, struct ep_address *out) {
    struct ep_table_entry *e = msgu_map_get(&t->entries, &epid);
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


int ep_table_notify_read(struct ep_table *t) {
    int wd = ep_notify_read(t->inotify_fd);
    struct ep_table_watch *w = msgu_map_get(&t->watched, &wd);
    if (w) {
        return w->epid;
    }
    else {
        return 0;
    }
}


size_t ep_table_read(struct ep_table *t, int epid, char *buf, size_t count) {
    struct ep_table_entry *e = msgu_map_get(&t->entries, &epid);
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


size_t ep_table_read_buf(struct ep_table *t, int epid, struct msgu_buffer *b) {
    struct ep_table_entry *e = msgu_map_get(&t->entries, &epid);
    int wr;
    if (e) {
        switch(e->type) {
        case ep_type_channel:
            wr = ep_buffer_take(b, e->data.ch.fd);
            if (wr < 0) {
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
    struct ep_table_entry *e = msgu_map_get(&t->entries, &epid);
    if (e) {
        switch(e->type) {
        case ep_type_channel:
            return ep_channel_fwd(t, epid, &e->data.ch);
        }
    }
    else {
        printf("%d not found\n", epid);
    }
}


void ep_channel_fwd(struct ep_table *t, int epid, struct ep_channel *c) {
    char buf [1024];
    int r = read(c->fd, buf, 1024);
    if (r > 0) {
        struct msgu_subarray *sa = msgu_multimap_get_key(&t->chanout, epid);
        for (int i = sa->begin; i < sa->end; ++i) {
            int out_epid = (int) t->chanout.values[sizeof(int) * i];
            struct ep_table_entry *e = msgu_map_get(&t->entries, &out_epid);
            if (e) {
                ep_entry_write_blk(e, buf, r);
            }
        }
    }
}


struct msgu_buffer *ep_entry_get_buffer(struct ep_table_entry *e) {
    switch(e->type) {
    case ep_type_handler:
        return &e->data.hdl.buf;
    case ep_type_channel:
        return &e->data.ch.write_buf;
    }
    return NULL;
}


size_t ep_entry_write_buf(struct ep_table_entry *e, struct msgu_buffer *b, size_t start) {
    switch(e->type) {
    case ep_type_channel:
        return ep_buffer_write(b, e->data.ch.fd, start);
    case ep_type_handler:
        return ep_buffer_copy(&e->data.hdl.buf, b, start);
    }
    return 0;
}


size_t ep_entry_write_blk(struct ep_table_entry *e, char *b, size_t count) {
    switch(e->type) {
    case ep_type_channel:
        return write(e->data.ch.fd, b, count);
    case ep_type_handler:
        return ep_buffer_insert(&e->data.hdl.buf, b, count);
    }
    return 0;
}

#include <stdio.h>
#include <unistd.h>

#include "poll.h"
#include "queue.h"


void ep_event_queue_init(struct ep_event_queue *q, struct ep_table *t, size_t max_queue) {
    q->table = t;
    q->event = malloc(sizeof(struct ep_event) * max_queue);
    q->begin = 0;
    q->size = 0;
    q->avail = max_queue;
    q->next_id = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->empty, NULL);
}


void ep_queue_pop(struct ep_event_queue *q, struct ep_event *e) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->empty, &q->mutex);
    }
    *e = q->event[q->begin];
    q->begin = (q->begin + 1) % q->avail;
    --q->size;
    pthread_mutex_unlock(&q->mutex);
}


void ep_queue_push(struct ep_event_queue *q, struct ep_event *e) {
    pthread_mutex_lock(&q->mutex);
    size_t i = (q->begin + q->size) % q->avail;
    struct ep_event *back = &q->event[i];
    *back = *e;
    ++q->size;
    pthread_cond_broadcast(&q->empty);
    pthread_mutex_unlock(&q->mutex);
}


void ep_queue_from_table(struct ep_event_queue *q) {
    struct ep_event ev;
    int src [32];
    while (1) {
        int r = ep_poll_wait(q->table->epoll_fd, src, 32);
        for (int i = 0; i < r; ++i) {
            ev.epid = src[i];
            ev.srcid = 0;
            ep_queue_push(q, &ev);
        }
    }
}


void ep_queue_update(struct ep_event_queue *q, struct ep_event *ev) {
    struct ep_table_entry *e = ep_map_get(&q->table->entries, ev->epid);
    if (e) {
        switch(e->type) {
        case ep_type_acceptor:
            ep_queue_accept(q->table, &e->data.acc);
            break;
        case ep_type_channel:
            ep_queue_read_ch(q, ev, &e->data.ch);
            break;
        case ep_type_handler:
            ep_queue_read_hdl(q, ev, &e->data.hdl);
            break;
        }
    }
}


int ep_queue_accept(struct ep_table *t, struct ep_acceptor *a) {
    struct ep_channel newc;
    newc.addr.len = 32;
    newc.fd = accept(a->fd, (struct sockaddr *) &newc.addr.data, &newc.addr.len);
    if (newc.fd < 0) {
        perror("accept");
        return 0;
    }

    // init handler and output channels
    // TODO add to accepted multimap
    int epid = ep_add_channel(t, &newc);
    a->on_accept(t, epid, a->data);
    return epid;
}


void ep_queue_read_ch(struct ep_event_queue *q, struct ep_event *ev, struct ep_channel *c) {
    char buf [1024];
    struct ep_sink s;
    s.q = q;
    s.srcid = ev->epid;
    int r = read(c->fd, buf, 1024);
    if (r > 0) {
        struct ep_subarray *sa = ep_multimap_get_key(&q->table->chanout, ev->epid);
        for (int i = sa->begin; i < sa->end; ++i) {
            s.epid = (int) q->table->chanout.values[sizeof(int) * i];
            ep_write_blk(&s, buf, r);
        }
    }
}


void ep_queue_read_hdl(struct ep_event_queue *q, struct ep_event *ev, struct ep_handler *h) {
    // ensure only one thread enters the callback
    if (pthread_mutex_trylock(&h->mutex)) {
        // lock failed, requeue the event
        ep_queue_push(q, ev);
    }
    else {
        if (h->min_input <= h->buf.size) {
            struct ep_event_view v;
            v.src = ev->srcid;
            v.epid = ev->epid;
            v.self = h;
            h->callback(0, &v);
        }
        pthread_mutex_unlock(&h->mutex);
    }
}


void ep_queue_notify(struct ep_event_queue *q, struct ep_table_entry *e, int srcid) {
    struct ep_event ev;
    ev.epid = e->epid;
    ev.srcid = srcid;
    switch(e->type) {
    case ep_type_handler:
        ep_queue_push(q, &ev);
        break;
    }
}


size_t ep_write_buf(struct ep_sink *s, struct ep_buffer *b, size_t start) {
    struct ep_table_entry *e = ep_map_get(&s->q->table->entries, s->epid);
    if (e) {
        size_t wr = ep_entry_write_buf(e, b, start);
        ep_queue_notify(s->q, e, s->srcid);
        return wr;
    }
    else {
        printf("%d not found\n", s->epid);
    }
    return 0;
}


size_t ep_write_blk(struct ep_sink *s, char *b, size_t count) {
    struct ep_table_entry *e = ep_map_get(&s->q->table->entries, s->epid);
    if (e) {
        size_t wr = ep_entry_write_blk(e, b, count);
        ep_queue_notify(s->q, e, s->srcid);
        return wr;
    }
    else {
        printf("%d not found\n", s->epid);
    }
    return 0;
}

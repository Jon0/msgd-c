#include "queue.h"

void ep_event_queue_init(struct ep_event_queue *q, struct ep_table *t, size_t max_queue) {
    q->table = t;
    q->event = malloc(sizeof(struct ep_event) * max_queue);
    q->begin = 0;
    q->size = 0;
    q->avail = max_queue;
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


void ep_queue_from_table(struct ep_event_queue *q, struct ep_table *t) {
    struct ep_event ev;
    struct ep_source *src [32];
    while (1) {
        int r = ep_table_wait(t, src, 32);
        for (int i = 0; i < r; ++i) {
            ev.hdl = ep_table_hdl(t, src[i]->epid);
            ev.recv = src[i]->func;
            ep_queue_push(q, &ev);
        }
    }
}


void ep_write_buf(struct ep_event_queue *q, int epid, struct ep_buffer *b) {

}


void ep_write_fd(struct ep_event_queue *q, int epid, int fd) {

}


void ep_read_src(struct ep_event_queue *q, struct ep_source *src) {
    src->func(q->table, src->epid, src->out);

    // if the source has output
    if (src->out) {
        struct ep_event ev;
        ep_queue_push(q, &ev);
    }
}


void ep_apply_event(struct ep_event_queue *q, struct ep_event *e) {
    // if the epid has a source
    // then the input must be read
    if (e->src) {
        // queues another update
        ep_write_fd(q, e->src->out, e->src->fd);


    }
    else if (e->hdl) {
        // if the event has a handler
        struct ep_event_view v;
        v.queue = q;
        v.self = e;
        ep_handler_update(e->hdl, &v, e->attr);
    }
}

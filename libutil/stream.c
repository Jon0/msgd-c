#include <stdio.h>

#include "stream.h"


ssize_t msgu_buffer_read_fn(msgu_stream_id_t id, void *buf, size_t count) {
    ssize_t e = msgu_buffer_read(id.ptr, buf, count);
    printf("buffer_read: %d\n", e);
    return e;
}


ssize_t msgu_buffer_write_fn(msgu_stream_id_t id, const void *buf, size_t count) {
    ssize_t e = msgu_buffer_write(id.ptr, buf, count);
    printf("buffer_write: %d\n", e);
    return e;
}


void msgu_stream_init(struct msgu_stream *s, struct msgu_stream_fn *fn, msgu_stream_id_t id) {
    s->fn = fn;
    s->id = id;
    s->state = msgu_stream_open;
}


void msgu_stream_free(struct msgu_stream *s) {
    // nothing to free
}


int msgu_stream_is_open(const struct msgu_stream *stream) {
    return (stream->state & msgu_stream_open);
}


ssize_t msgu_stream_read(struct msgu_stream *s, void *buf, size_t count) {
    if (s->state & msgu_stream_open) {
        ssize_t recv = s->fn->read(s->id, buf, count);
        if (recv < msgu_stream_waiting) {
            s->state = 0;
        }
        return recv;
    }
    else {
        return msgu_stream_not_open;
    }
}


ssize_t msgu_stream_write(struct msgu_stream *s, const void *buf, size_t count) {
    if (s->state & msgu_stream_open) {
        ssize_t sent = s->fn->write(s->id, buf, count);
        if (sent < msgu_stream_waiting) {
            s->state = 0;
        }
        return sent;
    }
    else {
        return msgu_stream_not_open;
    }
}


ssize_t msgu_stream_discard(struct msgu_stream *s, size_t count) {
    char c;
    for (int i = 0; i < count; ++i) {
        if (s->fn->read(s->id, &c, 1) != 1) {
            return -1;
        }
    }
    return count;
}

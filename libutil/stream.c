#include "stream.h"


void msgu_stream_init(struct msgu_stream *s, msgu_stream_id_t id, struct msgu_stream_fn *fn) {
    s->id = id;
    s->fn = fn;
    size_t memsize = 16;
    char *mem = malloc(memsize * 2);
    msgu_buffer_init(&s->read_buf, mem, memsize);
    msgu_buffer_init(&s->write_buf, &mem[memsize], memsize);
}


void msgu_stream_free(struct msgu_stream *s) {
    free(s->read_buf.ptr);
}


ssize_t msgu_stream_read(struct msgu_stream *s, void *buf, size_t count) {
    return s->fn->read(s->id, buf, count);
}


ssize_t msgu_stream_write(struct msgu_stream *s, const void *buf, size_t count) {
    return s->fn->write(s->id, buf, count);
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

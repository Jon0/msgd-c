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


void msgu_fragment_inc(struct msgu_fragment *f) {
    f->section_begin += f->progress;
    f->progress = 0;
    f->complete += 1;
}


void msgu_fragment_base_zero(struct msgu_fragment *f) {
    f->base = 0;
}


void msgu_fragment_base_inc(struct msgu_fragment *f) {
    f->base = f->complete;
}


int msgu_read_many(struct msgu_stream *stream, struct msgu_fragment *f, msgu_frag_read_t *fns, void **objs, size_t count) {
    for (int i = (f->complete - f->base); i < count; ++i) {
        int result = fns[i](stream, f, objs[i]);
        if (result <= 0) {
            return result;
        }
        else {
            msgu_fragment_inc(f);
        }
    }
    return count;
}


int msgu_write_many(struct msgu_stream *stream, struct msgu_fragment *f, msgu_frag_write_t *fns, const void **objs, size_t count) {
    for (int i = (f->complete - f->base); i < count; ++i) {
        int result = fns[i](stream, f, objs[i]);
        if (result <= 0) {
            return result;
        }
        else {
            msgu_fragment_inc(f);
        }
    }
    return count;
}

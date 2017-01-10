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


void msgu_fragment_reset(struct msgu_fragment *f, size_t count) {
    for (int i = 0; i < count; ++i) {
        f[i].index = 0;
        f[i].count = 0;
    }
}


size_t msgu_fragment_progress(struct msgu_fragment *f) {
    return f->count;
}


size_t msgu_fragment_advance(struct msgu_fragment *f, size_t count) {
    f->count += count;
    return f->count;
}


void msgu_fragment_inc(struct msgu_fragment *f) {
    f[0].index += 1;
    f[0].count += f[0].count;
    f[1].index = 0;
    f[1].count = 0;
}


int msgu_fragment_check(struct msgu_fragment *f, int result) {
    if (f->count >= result) {
        // message is completely read
        return f->count;
    }
    else if (f->count > 0) {
        // socket is still open but waiting
        return 0;
    }
    else {
        return result;
    }
}


void msgu_fragment_base_zero(struct msgu_fragment *f) {
    f->index = 0;
}


void msgu_fragment_base_inc(struct msgu_fragment *f) {
    f->index += 1;
}


int msgu_fragment_read_check(struct msgu_fragment *f, size_t count) {
    return f->count >= count;
}


int msgu_read_fixed(struct msgu_stream *in, struct msgu_fragment *f, void *obj, size_t count) {
    char *buf = (char *) obj;
    size_t begin = msgu_fragment_progress(f);
    size_t remain = count - begin;
    if (remain == 0) {
        return 1;
    }
    ssize_t rs = msgu_stream_read(in, buf, remain);
    if (rs > 0) {
        msgu_fragment_advance(f, rs);
    }
    else {
        return rs;
    }

    // check if read has completed
    if (f->count == count) {
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_write_fixed(struct msgu_stream *out, struct msgu_fragment *f, void *obj, size_t count) {
    char *buf = (char *) obj;
    size_t begin = msgu_fragment_progress(f);
    size_t remain = count - begin;
    if (remain == 0) {
        return 1;
    }
    ssize_t ws = msgu_stream_write(out, &buf[begin], remain);
    if (ws > 0) {
        msgu_fragment_advance(f, ws);
    }
    else {
        return ws;
    }

    // check if write has completed
    if (f->count == count) {
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_frag_read_t *fns, void **objs, size_t count) {
    for (int i = f->index; i < count; ++i) {
        int result = fns[i](in, &f[1], objs[i]);
        if (result > 0) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return count;
}


int msgu_write_many(struct msgu_stream *out, struct msgu_fragment *f, msgu_frag_write_t *fns, const void **objs, size_t count) {
    for (int i = f->index; i < count; ++i) {
        int result = fns[i](out, &f[1], objs[i]);
        if (result > 0) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return count;
}


int msgu_i32_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *i) {
    return msgu_read_fixed(dest, f, i, sizeof(int32_t));
}


int msgu_i32_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *i) {
    return msgu_write_fixed(dest, f, i, sizeof(int32_t));
}

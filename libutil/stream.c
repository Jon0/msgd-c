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
    // advance moves byte tracker forward
    // used when reading from non-fragment sources
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
    if (result > 0) {
        msgu_fragment_inc(f);
        return 1;
    }
    else {
        return result;
    }
}


int msgu_fragment_complete(struct msgu_fragment *f, int result, size_t count) {
    // check result when reading from non-fragment sources
    if (f->count >= count) {
        // message is completely read
        return msgu_stream_complete;
    }
    else if (result >= 0) {
        // socket is still open but waiting
        return msgu_stream_partial;
    }
    else {
        // an error occured
        return result;
    }
}


int msgu_read_fixed(struct msgu_stream *in, struct msgu_fragment *f, void *obj, size_t count) {
    char *buf = (char *) obj;
    size_t begin = msgu_fragment_progress(f);
    size_t remain = count - begin;
    if (remain == 0) {
        return msgu_stream_complete;
    }
    int read = msgu_stream_read(in, buf, remain);
    if (read > 0) {
        msgu_fragment_advance(f, read);
    }
    return msgu_fragment_complete(f, read, count);
}


int msgu_write_fixed(struct msgu_stream *out, struct msgu_fragment *f, const void *obj, size_t count) {
    char *buf = (char *) obj;
    size_t begin = msgu_fragment_progress(f);
    size_t remain = count - begin;
    if (remain == 0) {
        return msgu_stream_complete;
    }
    int ws = msgu_stream_write(out, &buf[begin], remain);
    if (ws > 0) {
        msgu_fragment_advance(f, ws);
    }
    return msgu_fragment_complete(f, ws, count);
}


int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_frag_read_t *fns, void **objs, size_t count) {
    for (int i = f->index; i < count; ++i) {
        int result = fns[i](in, &f[1], objs[i]);
        if (result == msgu_stream_complete) {
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
        if (result == msgu_stream_complete) {
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

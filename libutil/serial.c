#include "parser.h"
#include "serial.h"


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


int msgu_read_many(struct msgu_stream *in, struct msgu_fragment *f, msgu_obj_read_t *fns, void **objs, size_t count) {
    for (int i = f->index; i < count; ++i) {
        int result = fns[i](in, &f[1], objs[i]);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return msgu_stream_complete;
}


int msgu_write_many(struct msgu_stream *out, struct msgu_fragment *f, msgu_obj_write_t *fns, const void **objs, size_t count) {
    for (int i = f->index; i < count; ++i) {
        int result = fns[i](out, &f[1], objs[i]);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(f);
        }
        else {
            return result;
        }
    }
    return msgu_stream_complete;
}


int msgu_i32_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *i) {
    return msgu_read_fixed(dest, f, i, sizeof(int32_t));
}


int msgu_i32_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *i) {
    return msgu_write_fixed(dest, f, i, sizeof(int32_t));
}

#include <string.h>

#include "string.h"


void msgu_string_init(struct msgu_string *str) {
    str->buf = NULL;
    str->count = 0;
}


void msgu_string_alloc(struct msgu_string *str, size_t size) {
    str->buf = malloc(size);
    str->count = size;
}


size_t msgu_string_size(struct msgu_string *str) {
    return sizeof(str->count) + str->count;
}


ssize_t msgu_string_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_string *str) {
    ssize_t read_size = 0;

    if (f->progress < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - f->progress;
        char *countbuf = (char *) &str->count;
        read_size = msgu_stream_read(src, &countbuf[f->progress], remain);
        if (read_size < 0) {
            return read_size;
        }
        else if (read_size < remain) {
            f->progress += read_size;
            return read_size;
        }
        else {
            str->buf = malloc(str->count);
            f->known_size = sizeof(str->count) + str->count;
            f->progress += read_size;
        }
    }

    // read remaining string data
    size_t str_start = f->progress - sizeof(str->count);
    ssize_t bsize = msgu_stream_read(src, &str->buf[str_start], str->count - str_start);
    if (bsize < 0) {
        return -1;
    }
    else {
        f->progress += bsize;
    }

    if (f->progress == sizeof(str->count) + str->count) {
        f->complete = 1;
    }
    return read_size + bsize;
}


ssize_t msgu_string_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_string *str) {
    ssize_t write_size = 0;
    f->known_size = sizeof(str->count) + str->count;
    if (f->progress < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - f->progress;
        char *countbuf = (char *) &str->count;
        write_size = msgu_stream_write(dest, &countbuf[f->progress], remain);
        if (write_size < 0) {
            return write_size;
        }
        else if (write_size < remain) {
            f->progress += write_size;
            return write_size;
        }
        else {
            f->progress += write_size;
        }
    }

    // read remaining string data
    size_t str_start = f->progress - sizeof(str->count);
    ssize_t bsize = msgu_stream_write(dest, &str->buf[str_start], str->count - str_start);
    if (bsize < 0) {
        return -1;
    }
    else {
        f->progress += bsize;
    }

    if (f->progress == sizeof(str->count) + str->count) {
        f->complete = 1;
    }
    return write_size + bsize;
}


int msgu_string_read_frag(struct msgu_stream *dest, struct msgu_fragment *f, void *str) {
    return msgu_string_read(dest, f, str);
}


int msgu_string_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *str) {
    return msgu_string_write(dest, f, str);
}


hash_t msgu_string_hash(const void *p) {
    const struct msgu_string *str = p;
    return msgu_fast_hash(str->buf, str->count);
}


int msgu_string_cmp(const void *a, const void *b) {
    const struct msgu_string *str_a = a;
    const struct msgu_string *str_b = b;
    int min = (str_a->count < str_b->count) ? str_a->count : str_b->count;
    int cmp = memcmp(str_a->buf, str_b->buf, min);
    if (cmp) {
        return cmp;
    }
    else {
        return str_a->count - str_b->count;
    }
}

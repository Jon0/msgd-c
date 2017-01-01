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


ssize_t msgu_string_read(struct msgu_string *str, struct msgu_stream *src, size_t off) {
    ssize_t read_size = 0;

    if (off < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - off;
        char *countbuf = (char *) &str->count;
        read_size = msgu_stream_read(src, &countbuf[off], remain);
        if (read_size < 0) {
            return -1;
        }
        else if (read_size < remain) {
            return read_size;
        }
        else {
            str->buf = malloc(str->count);
            off += read_size;
        }
    }

    // read remaining string data
    size_t str_start = off - sizeof(str->count);
    ssize_t bsize = msgu_stream_read(src, &str->buf[str_start], str->count - str_start);
    if (bsize < 0) {
        return -1;
    }
    return read_size + bsize;
}


ssize_t msgu_string_write(struct msgu_string *str, struct msgu_stream *dest, size_t off) {
    ssize_t write_size = 0;
    if (off < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - off;
        char *countbuf = (char *) &str->count;
        write_size = msgu_stream_write(dest, &countbuf[off], remain);
        if (write_size < 0) {
            return -1;
        }
        else if (write_size < remain) {
            return write_size;
        }
        else {
            off += write_size;
        }
    }

    // read remaining string data
    size_t str_start = off - sizeof(str->count);
    ssize_t bsize = msgu_stream_write(dest, &str->buf[str_start], str->count - str_start);
    if (bsize < 0) {
        return -1;
    }
    return write_size + bsize;
}

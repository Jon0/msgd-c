#include <string.h>

#include "string.h"


void msgu_string_init(struct msgu_string *str) {
    str->buf = NULL;
    str->count = 0;
}


void msgu_string_alloc(struct msgu_string *str, size_t size) {
    str->count = size;
    str->buf = malloc(size);
    memset(str->buf, 0, size);
}


void msgu_string_free(struct msgu_string *str) {
    if (str->buf) {
        free(str->buf);
        str->buf = NULL;
    }
    str->count = 0;
}


void msgu_string_from_static(struct msgu_string *str, const char *cstr) {
    msgu_string_from_buffer(str, cstr, strlen(cstr) + 1);
}


void msgu_string_from_buffer(struct msgu_string *str, const char *cstr, size_t count) {
    msgu_string_init(str);
    msgu_string_alloc(str, count);
    memcpy(str->buf, cstr, count);
}


void msgu_string_copy(struct msgu_string *dest, const struct msgu_string *src) {
    msgu_string_from_buffer(dest, src->buf, src->count);
}


size_t msgu_string_split(struct msgu_string *dest, size_t count, const char *src, const char *delim) {
    char *dup = strdup(src);
    char *string = dup;
    char *token;
    size_t index = 0;
    while (((token = strsep(&string, delim)) != NULL) && (index < count)) {
        if (strlen(token) > 0) {
            msgu_string_from_static(&dest[index], token);
            ++index;
        }
    }
    free(dup);
    return index;
}


int msgu_string_compare(const struct msgu_string *a, const struct msgu_string *b) {
    return msgu_binary_cmp(a->buf, b->buf, a->count, b->count);
}


size_t msgu_string_size(const struct msgu_string *str) {
    return sizeof(str->count) + str->count;
}


int msgu_string_read(struct msgu_stream *src, struct msgu_fragment *f, struct msgu_string *str) {
    int read_size = 0;
    size_t progress = msgu_fragment_progress(f);
    if (progress < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - progress;
        char *countbuf = (char *) &str->count;
        read_size = msgu_stream_read(src, &countbuf[progress], remain);
        if (read_size < 0) {
            return read_size;
        }
        else if (read_size < remain) {
            msgu_fragment_advance(f, read_size);
            progress = msgu_fragment_progress(f);
            return 0;
        }
        else {
            str->buf = malloc(str->count);
            msgu_fragment_advance(f, read_size);
            progress = msgu_fragment_progress(f);
        }
    }

    // read remaining string data
    size_t str_start = progress - sizeof(str->count);
    read_size = msgu_stream_read(src, &str->buf[str_start], str->count - str_start);
    if (read_size > 0) {
        msgu_fragment_advance(f, read_size);
    }
    return msgu_fragment_complete(f, read_size, sizeof(str->count) + str->count);
}


int msgu_string_write(struct msgu_stream *dest, struct msgu_fragment *f, const struct msgu_string *str) {
    int write_size = 0;
    size_t progress = msgu_fragment_progress(f);
    if (progress < sizeof(str->count)) {
        size_t remain = sizeof(str->count) - progress;
        char *countbuf = (char *) &str->count;
        write_size = msgu_stream_write(dest, &countbuf[progress], remain);
        if (write_size < 0) {
            return write_size;
        }
        else if (write_size < remain) {
            msgu_fragment_advance(f, write_size);
            progress = msgu_fragment_progress(f);
            return 0;
        }
        else {
            msgu_fragment_advance(f, write_size);
            progress = msgu_fragment_progress(f);
        }
    }

    // write remaining string data
    size_t str_start = progress - sizeof(str->count);
    write_size = msgu_stream_write(dest, &str->buf[str_start], str->count - str_start);
    if (write_size > 0) {
        msgu_fragment_advance(f, write_size);
    }
    return msgu_fragment_complete(f, write_size, sizeof(str->count) + str->count);
}


size_t msgu_string_size_frag(const void *str) {
    return msgu_string_size(str);
}


int msgu_string_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *str) {
    return msgu_string_read(src, f, str);
}


int msgu_string_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *str) {
    return msgu_string_write(dest, f, str);
}


hash_t msgu_string_hash(const void *p) {
    const struct msgu_string *str = p;
    return msgu_fast_hash(str->buf, str->count);
}


int msgu_string_cmp(const void *a, const void *b) {
    return msgu_string_compare(a, b);
}

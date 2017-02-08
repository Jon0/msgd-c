#include <stdio.h>

#include "parser.h"


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
    f[0].count += f[1].count;
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


void msgu_parser_init(struct msgu_parser *parser, struct msgu_type *fns) {
    parser->fns = fns;
    msgu_fragment_reset(parser->read_state, MSGU_FRAGMENT_MAX);
    msgu_fragment_reset(parser->write_state, MSGU_FRAGMENT_MAX);
}


int msgu_parser_read(struct msgu_parser *parser, struct msgu_stream *stream, void *obj) {
    int result;
    if (msgu_stream_is_open(stream) == 0) {
        printf("reading closed stream\n");
        return -1;
    }

    do {
        result = parser->fns->read(stream, parser->read_state, obj);
        if (result == msgu_stream_complete) {
            msgu_fragment_reset(parser->read_state, MSGU_FRAGMENT_MAX);
            return 1;
        }
    }
    while (result > msgu_stream_waiting);

    // check for errors
    if (result <= msgu_stream_remote_closed) {
        return -1;
    }
    else {
        return 0;
    }
}


int msgu_parser_write(struct msgu_parser *parser, struct msgu_stream *stream, const void *obj) {
    int result;
    if (msgu_stream_is_open(stream) == 0) {
        printf("writing closed stream\n");
        return -1;
    }

    do {
        result = parser->fns->write(stream, parser->write_state, obj);
        if (result == msgu_stream_complete) {
            msgu_fragment_reset(parser->write_state, MSGU_FRAGMENT_MAX);
            return 1;
        }
    }
    while (result > msgu_stream_waiting);

    // check for errors
    if (result <= msgu_stream_remote_closed) {
        return -1;
    }
    else {
        return 0;
    }
}

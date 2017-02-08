#ifndef LIBUTIL_PARSER_H
#define LIBUTIL_PARSER_H

#include "serial.h"
#include "stream.h"

#define MSGU_FRAGMENT_MAX 8


/*
 * shows size and write progress
 */
struct msgu_fragment {
    size_t index;
    size_t count;
};


/*
 *
 */
void msgu_fragment_reset(struct msgu_fragment *f, size_t count);
size_t msgu_fragment_progress(struct msgu_fragment *f);
size_t msgu_fragment_advance(struct msgu_fragment *f, size_t count);
void msgu_fragment_inc(struct msgu_fragment *f);


/*
 * return 1 for no errors
 */
int msgu_fragment_check(struct msgu_fragment *f, int result);


/*
 * check at least count bytes have been transferred
 */
int msgu_fragment_complete(struct msgu_fragment *f, int result, size_t count);


struct msgu_parser {
    struct msgu_type        *fns;
    struct msgu_fragment     read_state [MSGU_FRAGMENT_MAX];
    struct msgu_fragment     write_state [MSGU_FRAGMENT_MAX];
    size_t max_depth;
};


void msgu_parser_init(struct msgu_parser *parser, struct msgu_type *fns);


/*
 * return 1 if object completed transfer
 * return 0 if waiting
 * return -1 if cannot continue parsing
 */
int msgu_parser_read(struct msgu_parser *parser, struct msgu_stream *stream, void *obj);
int msgu_parser_write(struct msgu_parser *parser, struct msgu_stream *stream, const void *obj);


#endif

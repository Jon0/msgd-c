#ifndef LIBUTIL_PARSER_H
#define LIBUTIL_PARSER_H

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


/*
 * transfer stream functions
 */
typedef size_t (*msgu_transfer_size_t)(const void *);
typedef int (*msgu_transfer_read_t)(struct msgu_stream *, struct msgu_fragment *, void *);
typedef int (*msgu_transfer_write_t)(struct msgu_stream *, struct msgu_fragment *, const void *);


/*
 * set of functions for serialising
 */
struct msgu_transfer_fn {
    msgu_transfer_size_t   size;
    msgu_transfer_read_t   read;
    msgu_transfer_write_t  write;
};


struct msgu_parser {
    struct msgu_transfer_fn *fns;
    struct msgu_stream      *stream;
    struct msgu_fragment     read_state [MSGU_FRAGMENT_MAX];
    struct msgu_fragment     write_state [MSGU_FRAGMENT_MAX];
    size_t max_depth;
};


void msgu_parser_init(struct msgu_parser *parser, struct msgu_transfer_fn *fns, struct msgu_stream *s);


/*
 * return 1 if object completed transfer
 * return 0 if waiting
 * return -1 if cannot continue parsing
 */
int msgu_parser_read(struct msgu_parser *parser, void *obj);
int msgu_parser_write(struct msgu_parser *parser, const void *obj);


#endif

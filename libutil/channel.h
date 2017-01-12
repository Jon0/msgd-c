#ifndef LIBUTIL_CHANNEL_H
#define LIBUTIL_CHANNEL_H

#include "share.h"
#include "stream.h"
#include "update.h"

#define MSGU_FRAGMENT_MAX 8


/*
 * channel messages have this header
 */
struct msgu_channel_header {
    int32_t           data_type;
    int32_t           arg_type;
    int32_t           size;
    int32_t           network_id;
    int32_t           host_id;
    int32_t           share_id;
};


/*
 * track completetion of a read or write
 */
struct msgu_status {
    struct msgu_fragment       stat [MSGU_FRAGMENT_MAX];
    struct msgu_channel_header head;
    union msgu_any_update      data;
    int                        ready;
};


void msgu_status_reset(struct msgu_status *stat, int ready);


/*
 * defines read and write state of a connection
 * does not contain connection settings
 */
struct msgu_channel {
    int                     mode;
    struct msgu_stream      stream;
    struct msgu_status      read;
    struct msgu_status      write;
};


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn);
int msgu_channel_is_local(struct msgu_channel *c);
int msgu_channel_is_closed(struct msgu_channel *c);
int msgu_channel_result(struct msgu_channel *c);


/*
 * return 1 if new updates are ready
 * move_update can then be called
 */
int msgu_channel_read(struct msgu_channel *c);
int msgu_channel_write(struct msgu_channel *c);


/*
 * move update to accessable location
 */
int msgu_channel_update_move(struct msgu_channel *c, int *update_type, union msgu_any_update *update);


/*
 * return 1 if completed
 */
int msgu_channel_update_send(struct msgu_channel *c, int update_type, union msgu_any_update *update);


/*
 * read update, return type of update or 0 for incomplete read, and -1 for errors
 */
int msgu_poll_update(struct msgu_stream *in, struct msgu_fragment *f, struct msgu_channel_header *h, union msgu_any_update *data);
int msgu_push_update(struct msgu_stream *out, struct msgu_fragment *f, struct msgu_channel_header *h, union msgu_any_update *data);


#endif

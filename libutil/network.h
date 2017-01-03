#ifndef LIBUTIL_NETWORK_H
#define LIBUTIL_NETWORK_H

#include "protocol.h"
#include "share.h"
#include "stream.h"
#include "update.h"


/*
 * defines attributes of a connection
 */
struct msgu_channel {
    int                     mode;
    struct msgu_stream      stream;
    struct msgu_read_status stat;
    int                     update_type;
    union msgu_any_update   update;
};


struct msgu_channel_set {

};


/*
 * a set of hosts
 */
struct msgu_network {
    int network_id;
    struct msgu_channel_set set;
};


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn);
int msgu_channel_is_local(struct msgu_channel *c);
int msgu_channel_is_closed(struct msgu_channel *c);


/*
 * return 1 if new updates are ready
 */
int msgu_channel_read(struct msgu_channel *c);


/*
 * copy update
 */
int msgu_channel_update_copy(struct msgu_channel *c, int *update_type, union msgu_any_update *update);
int msgu_channel_update_free(struct msgu_channel *c);


#endif

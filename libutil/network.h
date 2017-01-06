#ifndef LIBUTIL_NETWORK_H
#define LIBUTIL_NETWORK_H

#include "protocol.h"
#include "share.h"
#include "stream.h"
#include "update.h"


/*
 * defines read write state of a connection
 * does not contain connection settings
 */
struct msgu_channel {
    int                     mode;
    struct msgu_stream      stream;
    struct msgu_read_status stat;
    int                     update_type;
    union msgu_any_update   update;
};


/*
 * a set of hosts
 */
struct msgu_network {
    int network_id;
};


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn);
int msgu_channel_is_local(struct msgu_channel *c);
int msgu_channel_is_closed(struct msgu_channel *c);


/*
 * return 1 if new updates are ready
 */
int msgu_channel_read(struct msgu_channel *c);


/*
 * move update to accessable location
 */
int msgu_channel_update_move(struct msgu_channel *c, int *update_type, union msgu_any_update *update);


#endif

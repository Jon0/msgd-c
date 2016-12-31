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


/*
 * return type of update, or zero for incomplete read
 */
int msgu_channel_read(struct msgu_channel *c, union msgu_any_update *u);
int msgu_channel_is_local(struct msgu_channel *c);


#endif

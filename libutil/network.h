#ifndef LIBUTIL_NETWORK_H
#define LIBUTIL_NETWORK_H

#include "protocol.h"
#include "share.h"
#include "stream.h"




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


struct msgu_server {

};


void msgu_server_signal(struct msgu_server *s, struct msgu_channel *c);


#endif

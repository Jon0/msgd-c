#ifndef LIBUTIL_NETWORK_H
#define LIBUTIL_NETWORK_H

#include "share.h"


/*
 * defines attributes of a connection
 */
struct msgu_channel {

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


void msgu_apply_msg(struct msgu_network *n, struct msgu_channel *c);


#endif

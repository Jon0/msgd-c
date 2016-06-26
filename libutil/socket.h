#ifndef SOCKET_H
#define SOCKET_H

#include "channel.h"


/*
 * what if the channel moves address?
 */
struct socket_endpoint {
    struct channel *ch;
};

#endif

#ifndef LIBSYS_NETWORK_H
#define LIBSYS_NETWORK_H

#include <libshare/address.h>


struct msgu_host {
    struct msgu_address addr;
    char hostname [256];
};


void msgs_host_init_self(struct msgu_host *h);

#endif

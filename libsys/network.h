#ifndef LIBSYS_NETWORK_H
#define LIBSYS_NETWORK_H

#include <libshare/address.h>


/*
 * per remote host information
 * includes list of all available shares
 * Host may require multiple addresses,
 * one for each interface.
 */
struct msgu_host {
    int                 interface;
    struct msgu_address addr;
    char                hostname [256];
};


void msgs_host_init_self(struct msgu_host *h);


#endif

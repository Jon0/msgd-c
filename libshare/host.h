#ifndef LIBUTIL_HOST_H
#define LIBUTIL_HOST_H

#include <libutil/buffer.h>
#include <libutil/map.h>

#include "address.h"
#include "share.h"


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


struct msg_host_list {
    struct msgu_host *ptr;
    size_t host_count;
    size_t host_max;
};


int msg_host_init(struct msgu_host *h, const char *addr, const char *name);


int msg_host_list_init(struct msg_host_list *h);
int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name);
struct msgu_host *msg_host_match(struct msg_host_list *h, const char *hostname);


#endif

#ifndef LIBUTIL_ADDRESS_H
#define LIBUTIL_ADDRESS_H

#include <stdint.h>

#include <libutil/buffer.h>


struct msgu_ipc {
    size_t len;
    char *data;
};


struct msgu_ipv4 {
    short port;
    char data [4];
};


struct msgu_ipv6 {
    short port;
    char data [16];
};


/*
 * any type of address
 */
struct msgu_address {
    char   data [128];
    size_t len;
};


enum ep_host_flags {
    ep_host_ipv4 = 0x1,
    ep_host_ipv6 = 0x2
};


/*
 * interface connects to addresses
 * find a default
 */
struct ep_network_interface {

};


#endif

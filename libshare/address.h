#ifndef LIBUTIL_ADDRESS_H
#define LIBUTIL_ADDRESS_H

#include <stdint.h>

#include "buffer.h"


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
    char   data [32];
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


/*
 * host may require multiple addresses
 * one for each interface
 */
struct ep_host {
    char addr [32];
    char hostname [256];
    int32_t flags;
};


int ep_read_host(struct ep_host *h, struct msgu_buffer *b, size_t offset);
int ep_write_host(struct ep_host *h, struct msgu_buffer *b);

#endif

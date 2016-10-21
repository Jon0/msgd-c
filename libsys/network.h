#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

#include <libutil/buffer.h>


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


void ep_host_init_self(struct ep_host *h);


int ep_read_host(struct ep_host *h, struct ep_buffer *b, size_t offset);
int ep_write_host(struct ep_host *h, struct ep_buffer *b);

#endif

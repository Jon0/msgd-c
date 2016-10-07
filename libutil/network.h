#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#include "buffer.h"


enum ep_host_flags {
    ep_host_ipv4 = 0x1,
    ep_host_ipv6 = 0x2
};


struct ep_host {
    char addr [32];
    int32_t flags;
};


int ep_read_host(struct ep_host *h, struct ep_buffer *b);
int ep_write_host(struct ep_host *h, struct ep_buffer *b);

#endif

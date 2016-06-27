#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>


// endpoint is local, module or network
// local endpoints are file descriptors
enum ep_mode {
    local,
    remote,
    module
};


struct ep_attributes {
    int tls;
    int ipv4;
    int shared;
    char *name;
};


struct ep_packet {
    int epid;
    int size;
};


/*
 * a connected pair
 */
struct ep_pair {
    int remote_addr;
    int state;
};


struct ep_listener {
    struct ep_pair *pair;
    size_t count;
    int state;
};


struct ep_table {
    char *path;
};


/*
 * init the table with a path to store socket data
 */
void ep_init(struct ep_table *t, char *path);


/*
 * open new socket
 */
int ep_open(struct ep_table *t, int type);


/*
 * thread safe, non-blocking read
 */
void ep_poll(struct ep_table *t);





#endif

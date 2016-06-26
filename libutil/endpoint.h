#ifndef ENDPOINT_H
#define ENDPOINT_H


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


struct ep_table {

};


/*
 * open new socket
 */
int ep_open(struct ep_table *t, int type);


/*
 * thread safe, non-blocking read
 */
void ep_poll(struct ep_table *t);





#endif

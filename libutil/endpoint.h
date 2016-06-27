#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>

#include "thread.h"


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


/*
 * the initialised listeners and all current connections
 */
struct ep_listener {
    struct ep_pair  *pair;
    size_t           count;
    int              state;
};


/*
 * action to occur when recieving input
 * must contain a readable file descriptor
 */
struct ep_handler {
    int fd;
    int type;
    void (*fn)();
};


/*
 * map file descriptors to handlers
 */
struct ep_handler_map {
    struct ep_handler *handlers;
    size_t handler_count;
};


/*
 * list of file descriptors
 */
struct ep_table {
    char *path;
    struct pollfd *fds;
    nfds_t fdcount;
    struct ep_handler_map fdmap;
};


struct ep_table_delta {

};


void ep_trigger(struct ep_handler_map *map, int fd);

/*
 * init the table with a path to store socket data
 */
void ep_init(struct ep_table *t, char *path);


/*
 * open new socket
 */
int ep_open(struct ep_table *t, int type);


/*
 * thread safe, blocking read
 */
void ep_poll(struct ep_table *t, struct thread_pool *p);


/*
 * allows table to recieve changes
 */
void ep_modify(struct ep_table *t, struct ep_table_delta *dt);




#endif

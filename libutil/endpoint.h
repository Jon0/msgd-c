#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "buffer.h"
#include "hashmap.h"

struct ep_table;

/*
 * data visible inside each thread
 */
struct ep_event_view {
    struct ep_event_queue *queue;
    struct ep_event       *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(int, struct ep_event_view *);


/*
 * each epid is either an address, or a handler
 * memory allocated per active input?
 */
struct ep_handler {
    struct ep_buffer    buf;
    ep_callback_t       callback;
    size_t              min_input;
};


/*
 * move data from source to handlers
 * or accept new sockets
 * returns copied data
 */
typedef int (*ep_fwd_t)(struct ep_table *, int src, int out);
typedef int (*ep_accept_t)(struct ep_table *, int src);


struct ep_acceptor {
    int           fd;
    ep_accept_t   accept;
    ep_callback_t callback;
    ep_callback_t read;
};


/*
 * moves input from file descriptors into handlers
 */
struct ep_channel {
    int           fd;
    ep_fwd_t      func;
};


/*
 *
 */
union ep_source {
    struct ep_acceptor acc;
    struct ep_channel  chan;
};


/*
 * give address for a remote
 * epid is 0 for unused blocks
 */
struct ep_address {
    char               addr [256];
    socklen_t          addrlen;
    union ep_source    src;
};


union ep_item {
    struct ep_address addr;
    struct ep_handler hdl;
};



#endif

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "buffer.h"
#include "hashmap.h"

#define EP_OUT_MAX 32

struct ep_table;

/*
 * data visible inside each thread
 */
struct ep_event_view {
    struct ep_event_queue *queue;
    int                    epid;
    struct ep_handler     *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(int, struct ep_event_view *);

/*
 * create and return a handler for accepted sockets
 * this may need to return many handlers
 */
typedef int (*ep_accept_t)(struct ep_table *, int *);


/*
 * each epid is either an address, or a handler
 * memory allocated per active input?
 */
struct ep_handler {
    struct ep_buffer    buf;
    pthread_mutex_t     mutex;
    ep_callback_t       callback;
    size_t              min_input;
    void               *data;
};


/*
 * give address for a remote
 */
struct ep_address {
    char               data [32];
    socklen_t          len;
};


struct ep_acceptor {
    struct ep_address addr;
    int               fd;
    ep_accept_t       create_hdl;
};


/*
 * moves input from file descriptors into handlers
 */
struct ep_channel {
    struct ep_address addr;
    int               fd;
    int               output [EP_OUT_MAX];
    char              outcount;
};


void ep_handler_init(struct ep_handler *h, size_t size, ep_callback_t c);

#endif

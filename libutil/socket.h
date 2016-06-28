#ifndef SOCKET_H
#define SOCKET_H

#include <poll.h>

#include "endpoint.h"
#include "thread.h"


struct ep_accept_data {
    struct pollfd event;
    struct thread_pool *pool;
};


struct ep_read_data {
    struct pollfd event;
};


/*
 * a thread to handle input of a single file descriptor
 */
void *ep_socket_accept(void *p);
void *ep_socket_read(void *p);

#endif

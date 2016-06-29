#ifndef SOCKET_H
#define SOCKET_H

#include <poll.h>

#include "buffer.h"
#include "endpoint.h"
#include "thread.h"


struct ep_accept_data {
    struct pollfd src;
    notify_fn_t notify;
};


/*
 * a thread to handle input of a single file descriptor
 */
void *ep_thread_accept(void *p);

void ep_create_acceptor(struct ep_source *s, notify_fn_t fn);

#endif

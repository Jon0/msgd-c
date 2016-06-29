#ifndef SOCKET_H
#define SOCKET_H

#include <poll.h>

#include "buffer.h"
#include "endpoint.h"
#include "thread.h"


struct ep_accept_data {
    struct ep_table  *table;
    struct ep_source *tbsrc;
    notify_fn_t       notify_accept;
    notify_fn_t       notify_read;
};



/*
 * set endpoint to listen for local connections
 */
void ep_set_local(struct ep_source *src, const char *address);

/*
 * a thread to handle input of a single file descriptor
 */
void *ep_thread_accept(void *p);

void ep_activate_acceptor(struct ep_table *t, struct ep_source *s, notify_fn_t af, notify_fn_t rf);

void ep_activate_connector(struct ep_source *s, notify_fn_t rf);

#endif

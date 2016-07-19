#ifndef SOCKET_H
#define SOCKET_H

#include <poll.h>

#include "buffer.h"
#include "epoll.h"
#include "thread.h"


/*
 * accept new connections (acceptor address, socket address)
 */
typedef void (*notify_accept_t)(struct ep_address *, struct ep_address *);


struct ep_accept_data {
    struct ep_table   *table;
    struct ep_address *srcaddr;
    notify_accept_t    notify_accept;
};


/*
 * cleanup old sockets
 */
void ep_unlink(const char *address);

/*
 * set endpoint to listen for local connections
 */
void ep_set_local(struct ep_address *a, const char *address);
void ep_add_pipe_endpoints(struct ep_table *t, int epid);

/*
 * a thread to handle input of a single file descriptor
 */
void ep_on_accept(struct ep_table *t, int epid, union event_attr *e);

void ep_activate_acceptor(struct ep_address *a);

void ep_activate_connector(struct ep_address *a, notify_fn_t rf, void *obj);

/*
 *
 */
void ep_local_acceptor(struct ep_loop_data *d, struct ep_handler *h);

#endif

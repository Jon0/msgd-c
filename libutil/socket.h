#ifndef SOCKET_H
#define SOCKET_H

#include <poll.h>

#include "buffer.h"
#include "endpoint.h"
#include "thread.h"


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

void ep_activate_connector(struct ep_address *a);

/*
 *
 */
void ep_local_acceptor(struct ep_table *t, struct ep_handler *h);


#endif

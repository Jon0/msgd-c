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
void ep_set_remote(struct ep_address *a, short portnum);

/*
 * adds source and dest to table
 */
void ep_new_endpoints(struct ep_table *t, int epid, int type, ep_recv_t r);
void ep_activate_acceptor(struct ep_address *a);
void ep_activate_connector(struct ep_address *a);


/*
 * a thread to handle input of a single file descriptor
 */
void ep_on_accept_local(struct ep_table *t, int epid, union event_attr *e);
void ep_on_accept_net(struct ep_table *t, int epid, union event_attr *e);
void ep_on_recv(struct ep_table *t, int epid, union event_attr *e);


void ep_accept_endpoints(struct ep_table *t, int epid, int fd);


/*
 *
 */
void ep_tcp_acceptor(struct ep_table *t, struct ep_handler *h, ep_callback_t c);


#endif

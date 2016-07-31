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
void ep_listen_local(struct ep_address *a, const char *address);
void ep_listen_remote(struct ep_address *a, short portnum);
void ep_connect_remote(struct ep_address *a, const char *ip, short portnum);

/*
 * adds source and dest to table
 */
void ep_init_acceptor(struct ep_acceptor *a, ep_accept_t af, void *data);
int ep_init_channel(struct ep_channel *c);


#endif

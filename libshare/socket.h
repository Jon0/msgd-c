#ifndef LIBSHARE_SOCKET_H
#define LIBSHARE_SOCKET_H

#include "address.h"
#include "table.h"


/*
 * address and port
 */
struct msgu_sockaddr {
    struct msgu_address addr;
    short port;
};


typedef void (*connect_event)(void *, struct msgu_sockaddr *);
typedef void (*disconnect_event)(void *, struct msgu_sockaddr *);
typedef void (*recv_event)(void *, struct msgu_sockaddr *, struct msgu_message *);


/*
 * accepts sockets
 *
 */
struct msgu_acceptor {

};


#endif

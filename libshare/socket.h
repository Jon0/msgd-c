#ifndef LIBSHARE_SOCKET_H
#define LIBSHARE_SOCKET_H

#include <libutil/datatable.h>

#include "address.h"
#include "message.h"
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
    struct msgu_table *table;
    struct msgu_datatable data;
};


#endif

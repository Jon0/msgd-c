#ifndef LIBMSG_MSGHANDLER_H
#define LIBMSG_MSGHANDLER_H

#include "server.h"


/*
 * events from loopback interface
 */
struct msg_loopback {

};


/*
 * modify state, apply request.
 */
void msg_loopback_apply(struct msg_server *serv, struct msg_loopback *src, struct msgu_message *msg);


/*
 * events from socket acceptors
 */
struct msg_acceptor {

};





#endif

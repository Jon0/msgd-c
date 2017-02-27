#ifndef LIBMSG_PROCESS_H
#define LIBMSG_PROCESS_H

#include <libsys/fuse.h>
#include <libsys/network.h>
#include <libsys/thread.h>

#include "server.h"


/*
 * runs the server
 * controls interfaces which modify the server state
 */
struct msg_process {
    struct msg_system     external;
    struct msg_server     state;


    // listening sockets
    int                  local_acc_id;
    struct msgs_acceptor local_acc;
    int                  remote_acc_id;
    struct msgs_acceptor remote_acc;
};


/*
 * run generic server
 */
void msg_main_init(struct msg_process *proc, const char *sockpath);
void msg_main_run(struct msg_process *proc);


/*
 * interfaces for handling events
 */
int msg_server_recv_mount(struct msg_process *proc, struct msgu_mount_point *mnt, struct msgu_message *msg);

#endif

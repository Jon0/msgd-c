#ifndef LIBMSG_PROCESS_H
#define LIBMSG_PROCESS_H

#include <libsys/fuse.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>

#include "server.h"


/*
 * runs the server
 * controls interfaces which modify the server state
 */
struct msg_process {
    struct msgs_event_map emap;
    struct msgs_table     tb;
    struct msg_server     state;


    // listening sockets
    int                  local_acc_id;
    struct msgs_acceptor local_acc;
    int                  remote_acc_id;
    struct msgs_acceptor remote_acc;


    struct msgs_fuse_files *fuse;
};


/*
 * run generic server
 */
void msg_main_init(struct msg_process *proc, const char *sockpath);
void msg_main_run(struct msg_process *proc);


#endif

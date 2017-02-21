#ifndef LIBMSG_PROCESS_H
#define LIBMSG_PROCESS_H

#include <libsys/fuse.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>


#include "server.h"


struct msg_process {
    struct msg_server state;

    // event handling
    struct msgs_event_map emap;
    struct msgs_table     tb;

    // listening sockets
    int                  local_acc_id;
    struct msgs_acceptor local_acc;
    int                  remote_acc_id;
    struct msgs_acceptor remote_acc;


    struct msgs_fuse_files *fuse;
};


#endif

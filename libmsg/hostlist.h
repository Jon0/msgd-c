#ifndef LIBMSG_HOSTLIST_H
#define LIBMSG_HOSTLIST_H

#include "connection.h"


struct msg_host_link {
    int active;
    int status;
    struct msg_connection conn;
    struct msgu_string     name;
    struct msgu_string     addr;
};


struct msg_host_list {
    struct msg_host_link *links;
};


#endif

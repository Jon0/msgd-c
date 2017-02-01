#ifndef LIBMSG_HOSTLIST_H
#define LIBMSG_HOSTLIST_H

#include <libutil/datatable.h>

#include "connection.h"


/*
 *
 */
struct msg_host_link {
    int active;
    int status;
    struct msg_connection conn;
    struct msgu_string     name;
    struct msgu_string     addr;
};


/*
 * use table type, to index by many keys
 */
struct msg_host_list {
    msgs_mutex_t          list_mutex;
    struct msgu_datatable data;
};


void msgu_host_list_init(struct msg_host_list *l);


#endif

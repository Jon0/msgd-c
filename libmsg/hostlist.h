#ifndef LIBMSG_HOSTLIST_H
#define LIBMSG_HOSTLIST_H

#include <libutil/datatable.h>

#include "connection.h"


enum msg_host_status {
    msg_host_active = 0x01,
    msg_host_named  = 0x02,
};


/*
 *
 */
struct msg_host_link {
    uint64_t              status_bits;
    struct msg_connection conn;
};


/*
 * table containing id, address, name and link
 * use table type, to index by many keys
 */
struct msg_host_list {
    msgs_mutex_t          list_mutex;
    struct msgu_datatable data;
    struct msgu_datamap   id_map;
    struct msgu_datamap   addr_map;
    struct msgu_datamap   name_map;
};


void msgu_host_list_init(struct msg_host_list *l, size_t size);


#endif

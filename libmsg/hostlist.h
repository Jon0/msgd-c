#ifndef LIBMSG_HOSTLIST_H
#define LIBMSG_HOSTLIST_H

#include <libutil/datatable.h>
#include <libsys/eventmap.h>

#include "connection.h"
#include "system.h"


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
 * manages sockets but doesn't open them
 */
struct msg_host_list {
    msgs_mutex_t          list_mutex;
    msg_message_recv_t    recv_fn;
    void                 *recv_arg;
    struct msgu_datatable data;
    struct msgu_datamap   id_map;
    struct msgu_datamap   addr_map;
    struct msgu_datamap   name_map;
};


void msg_host_list_init(struct msg_host_list *list, size_t size, msg_message_recv_t rf, void *arg);


/*
 * manage connections
 */
int msg_hostlist_init_connection(struct msg_host_list *list, struct msg_system *ext, struct msgs_socket *socket);
int msg_hostlist_close_connection(struct msg_host_list *list, int id);
int msg_hostlist_name_connection(struct msg_host_list *list, int id, const struct msgu_string *name);
void msg_hostlist_print(struct msg_host_list *list);


/*
 * find connections, return NULL when not found
 */
struct msg_host_link *msg_hostlist_connection_id(struct msg_host_list *list, int id);
struct msg_host_link *msg_hostlist_connection_name(struct msg_host_list *list, const struct msgu_string *hostname);
int msg_hostlist_connection_notify(struct msg_host_list *list, int id);


/*
 * provides a locked mutex, to be unlocked when operations are completed
 */
struct msg_connection *msg_hostlist_use_id(struct msg_host_list *list, msgs_mutex_t **lock, int id);
struct msg_connection *msg_hostlist_use_host(struct msg_host_list *list, msgs_mutex_t **lock, const struct msgu_string *hostname);


/*
 * send to all active connections
 */
void msg_hostlist_broadcast(struct msg_host_list *list, int event_id, int event_type, int data_type, const union msgu_any_msg *data);

#endif

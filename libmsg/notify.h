#ifndef LIBMSG_NOTIFY_H
#define LIBMSG_NOTIFY_H

#include <libutil/datatable.h>
#include <libshare/message.h>

#include "connection.h"


struct msg_server;


/*
 * callback for recieved events
 */
typedef int (*msg_notify_fn_t)(struct msg_server *, void *, const struct msgu_message *);


/*
 * function to call when signalled
 * condition is optional
 */
struct msg_notify_state {
    int              state;
    void            *arg;
    msg_notify_fn_t  fn;
};


/*
 * sets notification functions
 */
struct msg_notify_map {
    struct msgu_datatable data;
    struct msgu_datamap   id_map;
};


void msg_notify_map_init(struct msg_notify_map *n, size_t size);


int msg_notify_map_add(struct msg_notify_map *n, int msg_id, msg_notify_fn_t fn, void *arg);
int msg_notify_map_signal(struct msg_notify_map *n, struct msg_connection *conn, const struct msgu_message *msg);


#endif

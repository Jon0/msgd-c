#ifndef LIBMSG_NOTIFY_H
#define LIBMSG_NOTIFY_H

#include <libutil/datatable.h>
#include <libshare/message.h>
#include <libsys/thread.h>

#include "connection.h"


typedef int (*msg_notify_fn_t)(void *, struct msg_connection *, const struct msgu_message *);


/*
 * function to call when signalled
 * condition is optional
 */
struct msg_notify_state {
    int              state;
    void            *arg;
    msg_notify_fn_t  fn;
    msgs_condition_t cond;
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

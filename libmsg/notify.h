#ifndef LIBMSG_NOTIFY_H
#define LIBMSG_NOTIFY_H

#include <libutil/datatable.h>
#include <libsys/thread.h>


/*
 * sets notification functions
 */
struct msg_notify_map {
    struct msgu_datatable data;
};


void msg_notify_map_init(struct msg_notify_map *n);


int msg_notify_map_add(struct msg_notify_map *n, int msg_id);
int msg_notify_map_wait(struct msg_notify_map *n, int msg_id);


#endif

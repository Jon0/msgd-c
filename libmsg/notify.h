#ifndef LIBMSG_NOTIFY_H
#define LIBMSG_NOTIFY_H

#include <libutil/datatable.h>


struct msg_notify_map {
    struct msgu_datatable data;
    int next_id;
};


void msg_notify_map_init(struct msg_notify_map *n);


#endif

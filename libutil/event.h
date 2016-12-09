#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include "stdlib.h"

#include "address.h"
#include "object.h"


struct msgu_timer_event {

};


struct msgu_connect_event {
    struct msgu_address local;
};


struct msgu_disconnect_event {

};


/*
 * events from ipc and other hosts, including disconnect events
 */
struct msgu_socket_event {
    struct msgu_address local;
    struct msgu_address remote;
    struct msgu_fd      fd;
};


/*
 * inotify
 */
struct msgu_file_event {

};


typedef int (*msgu_timer_event_callback)(struct msgu_timer_event *, void *);


/*
 * pass event type, which may be a disconnect
 */
typedef int (*msgu_connect_event_callback)(struct msgu_connect_event *, struct msgu_address *remote, void *);



struct msgu_callback_table {
    size_t event_id_size;
};


/*
 * lookup callback function using os event type
 */
void msgu_handle_event(const struct msgu_callback_table *table, void *os_event_id);


#endif

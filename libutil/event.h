#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include <stdlib.h>

#include "address.h"
#include "buffer.h"
#include "object.h"


/*
 * events from ipc and other hosts, including disconnect events
 */
struct msgu_open_socket {
    struct msgu_address local;
    struct msgu_address remote;
};


/*
 * events from files
 */
struct msgu_open_file {
    // TODO a path type
    const char *path;
};


struct msgu_timer_event {

};


struct msgu_connect_event {
    struct msgu_address local;
};


struct msgu_disconnect_event {

};



struct msgu_recv_event {
    struct msgu_fd      fd;
};


struct msgu_write_event {
    struct msgu_fd      fd;
};


/*
 * inotify
 */
struct msgu_file_event {

};


/*
 * list each specific event handler
 */
int msgu_local_connect(struct msgu_connect_event *);



/*
 * returns how much of the buffer was read
 */
size_t msgu_recv_event_callback(struct msgu_recv_event *, struct msgu_buffer *b, void *);


#endif

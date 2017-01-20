#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include <stdlib.h>

#include "address.h"
#include "buffer.h"
#include "map.h"
#include "update.h"


/*
 * id for each event type
 */
enum msgu_event_type {
    msgu_connect_id = 20,
    msgu_recv_id    = 40,
    msgu_send_id    = 41,
    msgu_file_id    = 60,
};


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
    int id;
};


struct msgu_connect_event {
    int id;
    struct msgu_address local;
};


/*
 * recieve updates to sockets, including socket getting closed
 */
struct msgu_recv_event {
    int                 id;
    struct msgu_buffer *buf;
};


struct msgu_send_event {
    int id;
};


/*
 * inotify
 */
struct msgu_file_event {
    int id;
};


/*
 * a client process which requires updates when events occur
 * can be set to ignore some update types
 */
struct msgu_proc_event {
    int epid;
    int subid;
    int event_mask;
};


union msgu_any_event {
    struct msgu_timer_event timer;
    struct msgu_connect_event conn;
    struct msgu_recv_event recv;
    struct msgu_send_event send;
    struct msgu_file_event file;
};


/*
 * list each specific event handler
 */
int msgu_local_connect(struct msgu_connect_event *ce);


/*
 * returns how much of the buffer was read
 */
size_t msgu_recv_event_callback(struct msgu_recv_event *re, struct msgu_buffer *b, void *p);


/*
 * callbacks for each event type
 */
struct msgu_handlers {
    void (*connect_event)(void *, struct msgu_connect_event *);
    void (*recv_event)(void *, struct msgu_recv_event *);
};


#endif

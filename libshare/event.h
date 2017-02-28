#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include <stdlib.h>

#include <libutil/buffer.h>
#include <libutil/map.h>

#include "address.h"
#include "update.h"


/*
 * id for each event type
 */
enum msgu_event_type {
    msgu_connect_id = 20,
    msgu_recv_id    = 40,
    msgu_send_id    = 41,
    msgu_share_id   = 60,
    msgu_mount_id   = 80,
};


enum msgu_mount_event_type {
    msgu_mount_event_attr,
    msgu_mount_event_list,
    msgu_mount_event_open,
    msgu_mount_event_read,
    msgu_mount_event_write,
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
};


struct msgu_send_event {
    int id;
};


/*
 * inotify
 */
struct msgu_share_event {
    int id;
};


/*
 * fuse
 */
struct msgu_mount_event {
    int event_type;
    const char *path;
    const char *data;
    size_t size;
    size_t offset;
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
    struct msgu_share_event share;
    struct msgu_mount_event mount;
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
 * TODO use messages, along with the event source
 */
struct msgu_handlers {
    void (*connect_event)(void *, struct msgu_connect_event *);
    void (*recv_event)(void *, struct msgu_recv_event *);
    void (*mount_event)(void *, struct msgu_mount_event *);
};


#endif

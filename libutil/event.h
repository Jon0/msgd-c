#ifndef LIBUTIL_EVENT_H
#define LIBUTIL_EVENT_H

#include <stdlib.h>

#include "address.h"
#include "buffer.h"
#include "map.h"


/*
 * id for each event type
 */
enum msgu_event_type {
    msgu_connect,
    msgu_disconnect,
    msgu_recv,
    msgu_send,
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


/*
 * contains a list of each type of event
 */
struct msgu_event_map {
    uint32_t               next_id;
    struct msgu_map        data;
    struct msgu_handlers   hdl;
    void                  *arg;
};


/*
 * event map functions
 */
void msgu_event_map_init(struct msgu_event_map *map, struct msgu_handlers *h, void *a);


/*
 * copy event data before callback gets used
 * returns 1 for success
 */
int msgu_event_copy(struct msgu_event_map *map, uint32_t id, union msgu_any_event *data);
void msgu_event_notify(struct msgu_event_map *map, uint32_t type, union msgu_any_event *data);


/*
 * returns the id of the new element
 */
int msgu_add_conn(struct msgu_event_map *map, struct msgu_connect_event *ce);
int msgu_add_recv(struct msgu_event_map *map, struct msgu_recv_event *re);
int msgu_add_file(struct msgu_event_map *map, struct msgu_file_event *fe);
int msgu_remove(struct msgu_event_map *map, uint32_t id);


#endif

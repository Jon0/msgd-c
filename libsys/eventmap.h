#ifndef LIBSYS_EVENTMAP_H
#define LIBSYS_EVENTMAP_H

#include <libutil/event.h>

#include "thread.h"


/*
 * contains a list of each type of event
 */
struct msgs_event_map {
    pthread_mutex_t        map_mutex;
    uint32_t               next_id;
    struct msgu_map        data;
    struct msgu_handlers   hdl;
    void                  *arg;
};


/*
 * event map functions
 */
void msgu_event_map_init(struct msgs_event_map *map, struct msgu_handlers *h, void *a);


/*
 * copy event data before callback gets used
 * returns 1 for success
 */
int msgu_event_copy(struct msgs_event_map *map, uint32_t id, union msgu_any_event *data);
void msgu_event_notify(struct msgs_event_map *map, uint32_t type, union msgu_any_event *data);


/*
 * called from table when new events arrive
 */
void msgs_event_recv(struct msgs_event_map *map, uint32_t ev, uint32_t type, uint32_t id);


/*
 * returns the id of the new element
 */
int msgu_add_connect_handler(struct msgs_event_map *map);
int msgu_add_recv_handler(struct msgs_event_map *map);
int msgu_add_file_handler(struct msgs_event_map *map);
int msgu_remove_handler(struct msgs_event_map *map, uint32_t id);

#endif

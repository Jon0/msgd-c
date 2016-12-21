#ifndef TABLE_H
#define TABLE_H

#include <libutil/map.h>
#include <libutil/event.h>

#include "endpoint.h"
#include "thread.h"


/*
 * Table of active event listeners,
 * making use of kernel epoll functionality.
 * Includes basic routing of recieved data to handlers
 *
 * no need to store list of endpoints here
 */
struct msgs_table {
    pthread_mutex_t        ctl_mutex;
    pthread_mutex_t        map_mutex;
    int                    epoll_fd;
    int                    inotify_fd;
    struct msgu_event_map *emap;
};


/*
 * init the table
 */
void msgs_table_init(struct msgs_table *t, struct msgu_event_map *map);
void msgs_table_free(struct msgs_table *t);


/*
 * loop waiting for events
 */
void msgs_table_start(struct msgs_table *t, int threads);
void msgs_table_poll(struct msgs_table *t);
void msgs_table_recv(struct msgs_table *t, uint32_t ev, uint32_t type, uint32_t id);


/*
 * read inotify events
 */
int ep_table_notify_read(struct msgs_table *t);


/*
 * add new file descriptors to epoll
 */
void msgs_table_enable(struct msgs_table *t, int fd, uint32_t type, uint32_t id);
int msgs_poll_acceptor(struct msgs_table *t, struct msgs_acceptor *acc);
int msgs_poll_socket(struct msgs_table *t, struct msgs_socket *sk);


#endif

#ifndef TABLE_H
#define TABLE_H

#include <libutil/map.h>
#include <libshare/event.h>

#include "endpoint.h"
#include "eventmap.h"
#include "thread.h"


/*
 * Table of active event listeners,
 * making use of kernel epoll functionality.
 * Includes basic routing of recieved data to handlers
 *
 * no need to store list of endpoints here
 * TODO table should control sockets and parsing of recieved messages
 */
struct msgs_table {
    pthread_mutex_t        ctl_mutex;
    int                    epoll_fd;
    int                    inotify_fd;
    struct msgs_event_map *emap;
};


/*
 * init the table
 */
void msgs_table_init(struct msgs_table *t, struct msgs_event_map *emap);
void msgs_table_free(struct msgs_table *t);


/*
 * loop waiting for events
 */
void msgs_table_start(struct msgs_table *t, int threads);
void msgs_table_poll(struct msgs_table *t);
void msgs_table_poll_one(struct msgs_table *t);


/*
 * read inotify events
 */
int ep_table_notify_read(struct msgs_table *t);


/*
 * add new file descriptors to epoll
 */
void msgs_table_enable(struct msgs_table *t, int fd, uint32_t type, uint32_t id);


/*
 * start responding to events
 * make sure handlers are configured first
 */
void msgs_poll_acceptor(struct msgs_table *t, struct msgs_acceptor *acc, int id);
void msgs_poll_socket(struct msgs_table *t, struct msgs_socket *sk, int id);
void msgs_poll_file(struct msgs_table *t, struct msgs_file *file, int id);


#endif

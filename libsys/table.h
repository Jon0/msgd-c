#ifndef TABLE_H
#define TABLE_H

#include <libutil/map.h>
#include <libutil/event.h>

#include "endpoint.h"
#include "thread.h"


/*
 * callbacks for each event type
 */
struct msgs_handlers {
    void (*connect_event)(void *, struct msgu_connect_event *);
};


/*
 * Table of active event listeners,
 * making use of kernel epoll functionality.
 * Includes basic routing of recieved data to handlers
 *
 * no need to store list of endpoints here
 */
struct msgs_table {
    pthread_mutex_t        mutex;
    struct msgs_handlers   hdl;
    void                  *arg;
    int                    epoll_fd;
    int                    inotify_fd;
    int                    next_id;
    struct msgu_event_list list;
};


/*
 * init the table
 */
void msgs_table_init(struct msgs_table *t, size_t max, struct msgs_handlers *h, void *arg);
void msgs_table_free(struct msgs_table *t);


/*
 * add new file descriptors to epoll
 */
void msgs_table_enable(int epfd, int fd, uint32_t type, uint32_t id);


/*
 * loop waiting for events
 */
void msgs_table_start(struct msgs_table *t, int threads);
void msgs_table_poll(struct msgs_table *t);
void msgs_table_recv(struct msgs_table *t, uint32_t e, uint32_t type, uint32_t id);


/*
 * returns the id of the new element
 */
int msgs_add_acceptor(struct msgs_table *t, struct msgs_acceptor *a);
int msgs_add_socket(struct msgs_table *t, struct msgs_socket *s);
int msgs_add_file(struct msgs_table *t, struct msgs_file *f);
int msgs_remove(struct msgs_table *t, int epid);


/*
 * read inotify events
 */
int ep_table_notify_read(struct msgs_table *t);


#endif

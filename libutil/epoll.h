#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

#include "queue.h"



/*
 * map file descriptors to events
 */
struct ep_hdl_link {
    int                fd;
    struct ep_event    ev;
};


/*
 * listens for external events for a subset of handlers
 */
struct ep_loop_data {
    int epoll_fd;
    struct ep_table table;
    struct ep_hdl_link *fd_map;
    struct ep_event_queue *queue;
};


void ep_loop_init(struct ep_loop_data *d, struct ep_event_queue *q);

/*
 * add data source
 */
void ep_loop_source(struct ep_loop_data *d, struct ep_handler *h, ep_recv_t r);


/*
 * wait for events
 */
void ep_loop_run(struct ep_loop_data *d);
void ep_loop_event(struct ep_loop_data *d, struct epoll_event *event);



#endif

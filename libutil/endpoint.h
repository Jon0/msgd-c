#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>
#include <netinet/in.h>

#include "thread.h"


// endpoint is local, module or network
// local endpoints are file descriptors
enum ep_mode {
    local,
    remote,
    module
};


struct ep_attributes {
    int tls;
    int ipv4;
    int shared;
    char *name;
};


/*
 * a source of new events
 */
struct ep_source {
    struct pollfd  ksrc;
    char           addr [256];
    socklen_t      addrlen;
    pthread_t      thread;
    int            state;
    void          *mem;
};


/*
 * writable endpoints
 */
struct ep_dest {
    char           addr [256];
    socklen_t      addrlen;
    // either a function or a file descriptor
};


/*
 * notify function type
 */
typedef void *(*notify_fn_t)(struct ep_source *);


/*
 * map file descriptors to handlers
 */
struct ep_handler_map {
    struct ep_handler *handlers;
    size_t handler_count;
};


/*
 * list of source file descriptors
 */
struct ep_table {
    struct ep_source *src;
    size_t src_avail;
    size_t src_count;
    char path [256];
};


/*
 * init the table with a path to store socket data
 */
void ep_table_init(struct ep_table *t, char *path);
void ep_table_free(struct ep_table *t);

/*
 * wait for all source threads to complete
 */
void ep_table_join(struct ep_table *t);

/*
 * create a new endpoint entry
 */
struct ep_source *ep_table_add(struct ep_table *t);

#endif

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>
#include <netinet/in.h>

#include "thread.h"
#include "hashmap.h"


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
    int epid;
    struct pollfd  ksrc;
    pthread_t      thread;
    int            state;
    void          *mem;
};


/*
 * writable endpoints
 */
struct ep_dest {
    int    epid;
    int    fd; // either a function or a file descriptor
    void  *mem;
};


/*
 * give address for a remote
 * epid is 0 for unused blocks
 */
struct ep_address {
    int                epid;
    char               addr [256];
    socklen_t          addrlen;
    struct ep_source  *src;
    struct ep_dest    *dest;
};


/*
 * notify function type
 */
typedef void *(*notify_fn_t)(struct ep_source *);


/*
 * list of source file descriptors
 * a hash map of epid to attributes
 * all epids have an address attribute
 */
struct ep_table {
    struct ep_address *addr;
    struct ep_source *src;
    struct ep_dest *dest;
    size_t avail;
    size_t src_count;
    size_t dest_count;
    int next_id;
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
 *
 */
size_t ep_table_hash(struct ep_table *t, int epid);

/*
 * create a new endpoint entry
 */
struct ep_address *ep_new_addr(struct ep_table *t);

/*
 * create src and dest attributes for existing entries
 */
struct ep_source *ep_new_src(struct ep_table *t, int epid);
struct ep_dest *ep_new_dest(struct ep_table *t, int epid);

/*
 * return existing entries
 */
struct ep_address *ep_table_addr(struct ep_table *t, int epid);
struct ep_source *ep_table_src(struct ep_table *t, int epid);
struct ep_dest *ep_table_dest(struct ep_table *t, int epid);

#endif

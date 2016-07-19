#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>
#include <netinet/in.h>

#include "buffer.h"
#include "hashmap.h"


/*
 * data visible inside each thread
 */
struct ep_event_view {
    struct ep_event_queue *queue;
    struct ep_event       *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(int, struct ep_event_view *);


/*
 * memory allocated per active input?
 */
struct ep_handler {
    int                 epid;
    struct ep_buffer    buf;
    ep_callback_t       callback;
    size_t              min_input;
};


/*
 * moves input from file descriptors into handlers
 */
struct ep_source {
    int   epid;
    int   fd;
};


/*
 * writable endpoints
 */
struct ep_dest {
    int    epid;
    int    fd; // either a function or a file descriptor
};


/*
 * give address for a remote
 * epid is 0 for unused blocks
 */
struct ep_address {
    int                epid;
    char               addr [256];
    socklen_t          addrlen;
    struct ep_source *src;
    struct ep_dest    *dest;
};


/*
 * notify function type
 */
typedef void (*notify_fn_t)(struct ep_address *, void *);


/*
 * list of source file descriptors
 * a hash map of epid to attributes
 * all epids have an address attribute
 */
struct ep_table {
    struct ep_address *addr;
    struct ep_source  *src;
    struct ep_dest    *dest;
    struct ep_handler *hdl;
    size_t avail;
    size_t src_count;
    size_t dest_count;
    size_t hdl_count;
    int next_id;
};


/*
 * block until at least n bytes are read
 */
int ep_read_block(struct ep_source *s, size_t n);

/*
 * init the table with a path to store socket data
 */
void ep_table_init(struct ep_table *t);
void ep_table_free(struct ep_table *t);


/*
 *
 */
size_t ep_table_hash(struct ep_table *t, int epid);

/*
 * create a new endpoint entry
 */
struct ep_address *ep_new_addr(struct ep_table *t);
struct ep_handler *ep_new_hdl(struct ep_table *t, ep_callback_t c);

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
struct ep_handler *ep_table_hdl(struct ep_table *t, int epid);


#endif

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <poll.h>
#include <netinet/in.h>

#include "thread.h"
#include "hashmap.h"


struct ep_attributes {
    int tls;
    int ipv4;
    int shared;
    char *name;
};


/*
 * general source type
 */
struct ep_src {
    struct ep_handler *hdl;
    void *obj;
    void (*copy)(void *, size_t);
};


/*
 * moves input from file descriptors into handlers
 */
struct ep_src_fd {
    struct pollfd      ksrc;
};


/*
 * waits for another thread to pass input
 */
struct ep_src_cond {
    pthread_cond_t cond;
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
    struct ep_src     *src;
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
    struct ep_source *src;
    struct ep_dest *dest;
    size_t avail;
    size_t src_count;
    size_t dest_count;
    int next_id;
    char path [256];
};


/*
 * block until any input is recieved
 */
int ep_wait(struct ep_src *s);

/*
 * block until at least n bytes are read
 */
int ep_read_block(struct ep_src *s, size_t n);

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

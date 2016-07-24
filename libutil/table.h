#ifndef TABLE_H
#define TABLE_H

#include "endpoint.h"
#include "hashmap.h"


enum ep_type {
    ep_type_acceptor,
    ep_type_channel,
    ep_type_handler;
};


struct ep_table_entry {
    int           epid;
    enum ep_type  type;
    union ep_item item;
};


/*
 * todo: include map of acceptors to channels
 * and channels to handlers
 */
struct ep_table {
    int                next_id;
    int                epoll_fd;
    struct ep_map      entries;
    struct ep_multimap accepted;
    struct ep_multimap chanout;
};


size_t ep_entry_hash(void *p);

/*
 * init the table
 */
void ep_table_init(struct ep_table *t);
void ep_table_free(struct ep_table *t);


int ep_open(struct ep_table *t, enum ep_type t);
void ep_close(struct ep_table *t, int epid);


/*
 * modify endpoints
 */
void ep_table_ctl(struct ep_table *t, int epid);


/*
 * wait for events and return the sources
 */
int ep_table_wait(struct ep_table *t, int *src, size_t count);


/*
 * add a file descriptor to epoll
 */
void ep_enable_src(struct ep_table *t, struct ep_source *s);


/*
 * use type of epid to decide how to handle
 */
void ep_table_update(struct ep_table *t, int epid);



#endif

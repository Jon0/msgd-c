#ifndef TABLE_H
#define TABLE_H

#include "endpoint.h"
#include "hashmap.h"


enum ep_type {
    ep_type_acceptor,
    ep_type_channel,
    ep_type_handler
};


union ep_item {
    struct ep_acceptor acc;
    struct ep_channel ch;
    struct ep_handler hdl;
};


struct ep_table_entry {
    int           epid;
    enum ep_type  type;
    union ep_item data;
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


int ep_entry_id(void *p);

/*
 * init the table
 */
void ep_table_init(struct ep_table *t, size_t max);
void ep_table_free(struct ep_table *t);


int ep_add_acceptor(struct ep_table *t, struct ep_acceptor *a);
int ep_add_channel(struct ep_table *t, struct ep_channel *a);
int ep_add_handler(struct ep_table *t, struct ep_handler *a);
void ep_close(struct ep_table *t, int epid);


/*
 * modify endpoints
 */
void ep_table_ctl(struct ep_table *t, int epid);

int ep_table_accept(struct ep_table *t, struct ep_acceptor *a);


/*
 * wait for events and return the sources
 */
int ep_table_wait(struct ep_table *t, int *src, size_t count);


/*
 * add a file descriptor to epoll
 */
void ep_enable_fd(struct ep_table *t, int epid, int fd);


/*
 * use type of epid to decide how to handle
 * how will it add new events to the queue?
 */
void ep_table_update(struct ep_table *t, int epid);



#endif

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
int ep_add_channel(struct ep_table *t, struct ep_channel *c);
int ep_add_handler(struct ep_table *t, struct ep_handler *h);
void ep_close(struct ep_table *t, int epid);


/*
 * modify endpoints
 */
void ep_table_ctl(struct ep_table *t, int in, int out);


/*
 * return the network address of one endpoint
 */
int ep_table_addr(struct ep_table *t, int epid, struct ep_address *out);


/*
 * wait for events and return the sources
 */
int ep_table_wait(struct ep_table *t, int *src, size_t count);


/*
 * add a file descriptor to epoll
 */
void ep_enable_fd(struct ep_table *t, int epid, int fd);


/*
 * read directly
 */
size_t ep_table_read(struct ep_table *t, int epid, char *buf, size_t count);
size_t ep_table_read_buf(struct ep_table *t, int epid, struct ep_buffer *b);


/*
 * move from readable endpoint to handlers
 */
void ep_table_fwd(struct ep_table *t, int epid);
void ep_channel_fwd(struct ep_table *t, struct ep_channel *c);


/*
 * write data to endpoints
 */
size_t ep_fwd_buf(struct ep_table_entry *e, struct ep_buffer *b, size_t start);
size_t ep_fwd_blk(struct ep_table_entry *e, char *b, size_t count);


#endif

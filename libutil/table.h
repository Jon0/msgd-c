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
 * Table of active connections,
 * making use of kernel epoll functionality.
 * Includes basic routing of recieved data to handlers
 */
struct ep_table {
    int                next_id;
    int                epoll_fd;

    // int -> struct ep_table_entry
    struct ep_map      entries;

    // int -> int[]
    struct ep_multimap accepted;

    // int -> int[]
    struct ep_multimap chanout;
};


/*
 * returns epid of struct ep_table_entry
 */
int ep_entry_id(void *p);


/*
 * init the table
 */
void ep_table_init(struct ep_table *t, size_t max);
void ep_table_free(struct ep_table *t);


/*
 * returns the id of the new element
 */
int ep_add_acceptor(struct ep_table *t, struct ep_acceptor *a);
int ep_add_channel(struct ep_table *t, struct ep_channel *c);
int ep_add_handler(struct ep_table *t, struct ep_handler *h);
void ep_close(struct ep_table *t, int epid);


/*
 * print status of one node
 */
void ep_table_print_id(struct ep_table *t, int epid);


/*
 * automatically send input from one epid to another
 */
void ep_table_route(struct ep_table *t, int in, int out);


/*
 * buffer to write new data to
 */
struct ep_buffer *ep_table_get_buffer(struct ep_table *t, int epid);


/*
 * return the network address of one endpoint
 */
int ep_table_addr(struct ep_table *t, int epid, struct ep_address *out);


/*
 * read directly
 */
size_t ep_table_read(struct ep_table *t, int epid, char *buf, size_t count);
size_t ep_table_read_buf(struct ep_table *t, int epid, struct ep_buffer *b);


/*
 * move from readable endpoint to handlers
 */
void ep_table_fwd(struct ep_table *t, int epid);
void ep_channel_fwd(struct ep_table *t, int epid, struct ep_channel *c);


/*
 * write data to endpoints
 */
size_t ep_entry_write_buf(struct ep_table_entry *e, struct ep_buffer *b, size_t start);
size_t ep_entry_write_blk(struct ep_table_entry *e, char *b, size_t count);


#endif

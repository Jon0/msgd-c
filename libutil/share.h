#ifndef LIBUTIL_SHARE_H
#define LIBUTIL_SHARE_H

#include <stdlib.h>


/*
 * functions on nodes
 */
typedef size_t (*ep_node_open_t)(void *);
typedef size_t (*ep_node_close_t)(void *);
typedef size_t (*ep_node_read_t)(void *, char *, size_t);
typedef size_t (*ep_node_write_t)(void *, const char *, size_t);


/*
 * per node file system functions
 */
struct ep_operations {
    ep_node_open_t open;
    ep_node_close_t close;
    ep_node_read_t read;
    ep_node_write_t write;
};


/*
 * objects shared on a host
 */
struct ep_share {
    int name;
    int type;
    void *data;
    struct ep_operations ops;
};


#endif

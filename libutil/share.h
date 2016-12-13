#ifndef LIBUTIL_SHARE_H
#define LIBUTIL_SHARE_H

#include <stdlib.h>


/*
 * functions on nodes
 * will cover most of the fuse functions
 */
typedef size_t (*ep_node_open_t)(void *);
typedef size_t (*ep_node_close_t)(void *);
typedef size_t (*ep_node_read_t)(void *, char *, size_t);
typedef size_t (*ep_node_write_t)(void *, const char *, size_t);
typedef size_t (*ep_node_list_t)(void *, const char *);


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
 * shares can be block devices, sockets or filesystems
 * shares can contain attribute metadata
 * to indicate uptime, availability
 */
struct ep_share {
    int name;
    int type;
    int label;
    int attributes;
    void *data;
    struct ep_operations ops;
};


#endif

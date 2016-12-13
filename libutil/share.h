#ifndef LIBUTIL_SHARE_H
#define LIBUTIL_SHARE_H

#include <stdlib.h>

#include "buffer.h"
#include "map.h"


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
struct ep_share_meta {
    int name;
    int type;
    int label;
    int attributes;
    void *data;
    struct ep_operations ops;
};


/*
 * shares as seen by other hosts
 * does not distinguish types
 */
struct msg_share_set {
    size_t share_count;
};


void ep_share_set_print(struct msg_share_set *set);
void ep_share_set_init(struct msg_share_set *set);
size_t ep_share_set_size(struct msg_share_set *set);
size_t ep_share_set_read(struct msg_share_set *set, struct ep_buffer *buf, size_t offset);
size_t ep_share_set_write(struct msg_share_set *set, struct ep_buffer *buf);


/*
 * only processes have epid
 */
struct msg_share_id {
    int id;
    int type;
    int epid;
    int index;
};


struct msg_share_proc {
    struct ep_share_meta meta;
    int epid;
    char *procname;
};


/*
 * requires listening for inotify events and incoming requests
 */
struct msg_share_file {
    struct ep_share_meta meta;
    char *path;
};


/*
 * net shares are usually owned by another machine
 */
struct msg_share_net {
    struct ep_share_meta meta;
    int epid;
    int address;
};


/*
 * map id to resources
 * for server side
 */
struct msg_share_server {
    struct msgu_map          id_map;
    struct msg_share_proc *procs;
    struct msg_share_file *files;
    size_t proc_shares;
    size_t file_shares;
    size_t next_id;
};


void msg_share_debug(struct msg_share_server *set);
int msg_share_set_init(struct msg_share_server *set);
int msg_share_proc(struct msg_share_server *set);
int msg_share_file(struct msg_share_server *set);


/*
 * handle one of the fuse filesystem requests
 */
int msg_share_lsdir(struct msg_share_server *set, int share_id);



#endif

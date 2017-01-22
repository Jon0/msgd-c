#ifndef LIBUTIL_SHARE_H
#define LIBUTIL_SHARE_H

#include <stdlib.h>

#include <libutil/map.h>
#include <libutil/queue.h>
#include <libutil/stream.h>
#include <libutil/string.h>

#include "node.h"



/*
 * functions on nodes
 * will cover most of the fuse functions
 */
typedef size_t (*msgu_node_open_t)(void *);
typedef size_t (*msgu_node_close_t)(void *);
typedef size_t (*msgu_node_read_t)(void *, char *, size_t);
typedef size_t (*msgu_node_write_t)(void *, const char *, size_t);
typedef size_t (*msgu_node_list_t)(void *, const char *);


/*
 * shared file system functions
 */
struct msgu_fs_operations {
    msgu_node_open_t open;
    msgu_node_close_t close;
    msgu_node_read_t read;
    msgu_node_write_t write;
};


/*
 * stored in map to find share types
 */
struct msgu_share_id {
    char type;
    size_t index;
};


/*
 * only processes have connection id
 */
struct msgu_share_proc {
    struct msgu_node   node;
    struct msgu_string procname;
    int                connection_id;
};


/*
 * requires listening for inotify events and incoming requests
 */
struct msgu_share_file {
    struct msgu_node   node;
    struct msgu_string share_path;
};


/*
 * net shares are usually owned by another machine
 */
struct msgu_share_net {
    struct msgu_node   node;
    int epid;
    int address;
};


/*
 * map share name strings to resources
 * for server side
 */
struct msgu_share_map {
    struct msgu_map        id_map;
    struct msgu_share_proc *procs;
    struct msgu_share_file *files;
    size_t proc_shares;
    size_t file_shares;
};


void msgu_share_debug(struct msgu_share_map *set);
int msgu_share_set_init(struct msgu_share_map *set);
int msgu_share_proc(struct msgu_share_map *set, const struct msgu_string *name);
int msgu_share_file(struct msgu_share_map *set, const struct msgu_string *name);


/*
 * create a node list
 */
void msgs_node_list_of_shares(struct msgu_share_map *set, struct msgu_queue *nodes);


/*
 * write share state to stream
 */
int msgu_share_list(struct msgu_share_map *set, struct msgu_stream *out);
int msgu_share_list_dir(struct msgu_share_map *set, struct msgu_stream *out);




#endif

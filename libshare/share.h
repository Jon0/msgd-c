#ifndef LIBUTIL_SHARE_H
#define LIBUTIL_SHARE_H

#include <stdlib.h>

#include <libutil/map.h>
#include <libutil/stream.h>
#include <libutil/string.h>
#include <libutil/vector.h>

#include "filepath.h"
#include "node.h"


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
 * for server side, to provide resource sources
 * TODO: sort by hosts
 */
struct msgu_share_map {
    struct msgu_file_ops   *file_fn;
    struct msgu_file_params params;
    struct msgu_map         id_map;
    struct msgu_share_proc *procs;
    struct msgu_share_file *files;
    size_t proc_shares;
    size_t file_shares;
};


void msgu_share_debug(struct msgu_share_map *set);
int msgu_share_set_init(struct msgu_share_map *set, struct msgu_file_ops *fns);
int msgu_share_proc(struct msgu_share_map *set, const struct msgu_string *name);
int msgu_share_file(struct msgu_share_map *set, const struct msgu_string *name);
int msgu_share_get_file(struct msgu_share_map *set, const struct msgu_string *name, struct msgu_share_file *fshare);


/*
 * create a node list
 */
void msgs_node_list_of_shares(struct msgu_share_map *set, struct msgu_vector *nodes);


#endif

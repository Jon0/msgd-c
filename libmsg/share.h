#ifndef LIBMSG_SHARE_H
#define LIBMSG_SHARE_H

#include <stdlib.h>

#include <libutil/hashmap.h>


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
    int epid;
};


/*
 * requires listening for inotify events and incoming requests
 */
struct msg_share_file {
    char *path;
};


/*
 * map id to resources
 */
struct msg_share_set {
    struct ep_map          id_map;
    struct msg_share_proc *procs;
    struct msg_share_file *files;
    size_t proc_shares;
    size_t file_shares;
    size_t next_id;
};


int msg_share_set_init(struct msg_share_set *set);
int msg_share_proc(struct msg_share_set *set);
int msg_share_file(struct msg_share_set *set);


/*
 * handle one of the fuse filesystem requests
 */
int msg_handle_request(struct msg_share_set *set, int share_id);

#endif

#ifndef LIBMSG_SHARE_H
#define LIBMSG_SHARE_H

#include <stdlib.h>

#include <libutil/buffer.h>
#include <libutil/map.h>


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
    int epid;
    char *procname;
};


/*
 * requires listening for inotify events and incoming requests
 */
struct msg_share_file {
    char *path;
};


/*
 * net shares are usually owned by another machine
 */
struct msg_share_net {
    int epid;
    int address;
};


/*
 * map id to resources
 * for server side
 */
struct msg_share_server {
    struct ep_map          id_map;
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

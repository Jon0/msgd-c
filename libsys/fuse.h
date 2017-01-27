#ifndef FUSE_H
#define FUSE_H

#include <pthread.h>

#include <libshare/mount.h>

#include "eventmap.h"


/*
 * set of hosts and mount nodes
 * contains default owner and permissions
 */
struct msgs_fuse_files {
    char         mountpoint [1024];
    pthread_t    fuse_thread;
    struct fuse *fuse;
    struct fuse_chan *ch;
    struct msgu_mount_map *mounts;
    struct msgs_event_map *emap;


};


void msgs_fuse_static_start(struct msgs_fuse_files *f, struct msgu_mount_map *map, struct msgs_event_map *em, const char *subdir);
int msgs_fuse_init(struct msgs_fuse_files *f, struct msgu_mount_map *map, struct msgs_event_map *em, const char *subdir);
void msgs_fuse_free(struct msgs_fuse_files *f);
void msgs_fuse_loop(struct msgs_fuse_files *f);


#endif

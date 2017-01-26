#ifndef FUSE_H
#define FUSE_H

#include <pthread.h>

#include <libshare/mount.h>


/*
 * set of hosts and mount nodes
 * contains default owner and permissions
 */
struct msgs_fuse_files {
    char mountpoint [1024];
    pthread_t fuse_thread;
    struct fuse *fuse;
    struct fuse_chan *ch;
    struct msgu_mount_map *map;
};


void msgs_fuse_static_start(struct msgs_fuse_files *f, struct msgu_mount_map *map, const char *subdir);
int msgs_fuse_init(struct msgs_fuse_files *f, struct msgu_mount_map *map, const char *subdir);
void msgs_fuse_free(struct msgs_fuse_files *f);
void msgs_fuse_loop(struct msgs_fuse_files *f);


#endif

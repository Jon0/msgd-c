#ifndef FUSE_H
#define FUSE_H

#include <pthread.h>


/*
 * create functions to implement in the server
 */
typedef int (*msgs_fuse_file_read_t)(void *, int, char *, size_t);


struct msgs_fuse_files_fn {
    msgs_fuse_file_read_t read;
};


/*
 * map ids to opened fuse file
 */
struct msgs_fuse_opened {
    int id;
};


/*
 * set of hosts and mount nodes
 * contains default owner and permissions
 */
struct msgs_fuse_files {
    struct msgs_fuse_files_fn fns;
    char mountpoint [1024];
    pthread_t fuse_thread;
    struct fuse *fuse;
    struct fuse_chan *ch;
    struct msgu_map opened;
};


void msgs_fuse_set_dir(struct msgs_fuse_files *f, const char *subdir);
int msgs_fuse_init(struct msgs_fuse_files *f);
void msgs_fuse_free(struct msgs_fuse_files *f);
void msgs_fuse_loop(struct msgs_fuse_files *f);


#endif

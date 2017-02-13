#ifndef FUSE_H
#define FUSE_H

#include <pthread.h>

#include "eventmap.h"


struct msgs_fuse_response {
    int ready;
    int exists;
    struct msgu_node node;
    struct msgu_vector nodes;
    struct msgu_string data;
};


/*
 * set of hosts and mount nodes
 * contains default owner and permissions
 */
struct msgs_fuse_files {
    char            mountpoint [1024];
    pthread_t       fuse_thread;
    pthread_cond_t  fuse_cond;
    pthread_mutex_t fuse_mutex;
    struct fuse    *fuse;
    struct fuse_chan *ch;
    struct msgs_event_map *emap;
    struct msgs_fuse_response reply;
};


struct msgs_fuse_files *msgs_fuse_static_start(struct msgs_event_map *em, const char *subdir);
int msgs_fuse_init(struct msgs_fuse_files *f, struct msgs_event_map *em, const char *subdir);
void msgs_fuse_free(struct msgs_fuse_files *f);
void msgs_fuse_loop(struct msgs_fuse_files *f);
void msgs_fuse_notify(struct msgs_fuse_files *f);


#endif

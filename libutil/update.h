#ifndef LIBUTIL_UPDATE_H
#define LIBUTIL_UPDATE_H

#include "msgtype.h"
#include "stream.h"
#include "string.h"


/*
 * should be sent first to setup connection
 */
struct msgu_init_local_update {
     int32_t            version_maj;
     int32_t            version_min;
};



struct msgu_init_remote_update {
    int32_t            version_maj;
    int32_t            version_min;
    struct msgu_string host_name;
};


/*
 * add / remove hosts
 */
struct msgu_host_update {

};


/*
 * adds or removes local shares
 */
struct msgu_add_share_update {
    struct msgu_string share_name;
};


/*
 * updates state of a local share
 */
struct msgu_share_update {
    int share_id;
    int operation;
    int data;
};


struct msgu_add_peer_update {

};


/*
 *
 */
struct msgu_local_update {
    struct msgu_share_update shares;
};


/*
 *
 */
struct msgu_remote_update {
    struct msgu_share_update shares;
};


/*
 * events sent to remotes of changes
 */
struct msgu_broadcast_update {

};


union msgu_any_update {
    struct msgu_init_local_update  init_local;
    struct msgu_init_remote_update init_remote;
    struct msgu_add_share_update   sh_add;
    struct msgu_share_update       share;
};


/*
 * functions for each update type
 */
size_t msgu_init_local_size(struct msgu_init_local_update *u);
int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u);
int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u);


size_t msgu_init_remote_size(struct msgu_init_remote_update *u);
int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u);
int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u);


size_t msgu_add_share_size(struct msgu_add_share_update *u);
int msgu_add_share_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_add_share_update *u);
int msgu_add_share_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_add_share_update *u);


/*
 * return message size
 */
size_t msgu_update_size(int type, union msgu_any_update *u);


/*
 * print type
 */
void msgu_update_print(int type, union msgu_any_update *u);


/*
 * free memory
 */
void msgu_update_free(int type, union msgu_any_update *u);


#endif

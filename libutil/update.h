#ifndef LIBUTIL_UPDATE_H
#define LIBUTIL_UPDATE_H

#include "msgtype.h"
#include "node.h"
#include "stream.h"
#include "string.h"


/*
 * updates with no arguments
 */
struct msgu_empty_update {};


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
 * add, remove and list shares
 */
struct msgu_share_file_update {
    struct msgu_string share_name;
};


/*
 * operations requiring a path
 */
struct msgu_share_path_update {
    struct msgu_string share_name;
    struct msgu_string file_name;
};


/*
 * a list of nodes
 */
struct msgu_node_list_update {
    struct msgu_node_list nodes;
};


/*
 * events sent to remotes of changes
 */
struct msgu_broadcast_update {

};


union msgu_any_update {
    struct msgu_empty_update       empty;
    struct msgu_init_local_update  init_local;
    struct msgu_init_remote_update init_remote;
    struct msgu_share_file_update  share_file;
    struct msgu_share_path_update  share_path;
    struct msgu_node_list_update   node_list;
};


/*
 * functions for each update type
 */
size_t msgu_empty_size(struct msgu_empty_update *u);
int msgu_empty_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u);
int msgu_empty_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u);




size_t msgu_init_local_size(struct msgu_init_local_update *u);
int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u);
int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u);


size_t msgu_init_remote_size(struct msgu_init_remote_update *u);
int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u);
int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u);


size_t msgu_share_file_size(struct msgu_share_file_update *u);
int msgu_share_file_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_update *u);
int msgu_share_file_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_update *u);


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

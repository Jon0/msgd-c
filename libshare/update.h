#ifndef LIBUTIL_UPDATE_H
#define LIBUTIL_UPDATE_H

#include <libutil/queue.h>
#include <libutil/stream.h>
#include <libutil/string.h>

#include "msgtype.h"
#include "node.h"


/*
 * types of data used in messages
 */
enum msgu_data_type {
    msgdata_empty,
    msgdata_host_addr,
    msgdata_init_local,
    msgdata_init_remote,
    msgdata_share_file,
    msgdata_share_path,
    msgdata_node_list,
    msgdata_node_handle,
    msgdata_node_read,
    msgdata_node_write,
};


const char *msgu_msgdata_str(const enum msgu_data_type type);


/*
 * updates with no arguments
 */
struct msgu_empty_msg {};


/*
 * send ip address
 */
struct msgu_host_addr_msg {
     struct msgu_string address_str;
};


/*
 * should be sent first to setup connection
 */
struct msgu_init_local_msg {
     int32_t            version_maj;
     int32_t            version_min;
     struct msgu_string proc_name;
};



struct msgu_init_remote_msg {
    int32_t            version_maj;
    int32_t            version_min;
    struct msgu_string host_name;
};


/*
 * add, remove and list shares
 */
struct msgu_share_file_msg {
    struct msgu_string share_name;
};


/*
 * operations requiring a path
 */
struct msgu_share_path_msg {
    struct msgu_string share_name;
    struct msgu_string file_name;
};


/*
 * a list of nodes
 */
struct msgu_node_list_msg {
    struct msgu_queue nodes;
};


/*
 * sends id of a handle
 */
struct msgu_node_handle_msg {
    uint32_t node_handle;
};


/*
 * read data request
 */
struct msgu_node_read_msg {
    uint32_t node_handle;
    uint32_t count;
};


/*
 * write data request
 */
struct msgu_node_write_msg {
    uint32_t node_handle;
    struct msgu_string data;
};


/*
 * events sent to remotes of changes
 */
struct msgu_broadcast_msg {

};


union msgu_any_msg {
    struct msgu_empty_msg       empty;
    struct msgu_host_addr_msg   host_addr;
    struct msgu_init_local_msg  init_local;
    struct msgu_init_remote_msg init_remote;
    struct msgu_share_file_msg  share_file;
    struct msgu_share_path_msg  share_path;
    struct msgu_node_list_msg   node_list;
    struct msgu_node_handle_msg node_handle;
    struct msgu_node_read_msg   node_read;
    struct msgu_node_write_msg  node_write;
};


struct msgu_msgdata {
    int32_t            data_type;
    union msgu_any_msg data;
};


/*
 * functions for each update type
 */
size_t msgu_empty_size(const struct msgu_empty_msg *u);
int msgu_empty_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_msg *u);
int msgu_empty_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_empty_msg *u);


size_t msgu_host_addr_size(const struct msgu_host_addr_msg *u);
int msgu_host_addr_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_host_addr_msg *u);
int msgu_host_addr_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_host_addr_msg *u);


size_t msgu_init_local_size(const struct msgu_init_local_msg *u);
int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_msg *u);
int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_init_local_msg *u);


size_t msgu_init_remote_size(const struct msgu_init_remote_msg *u);
int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_msg *u);
int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_init_remote_msg *u);


size_t msgu_share_file_size(const struct msgu_share_file_msg *u);
int msgu_share_file_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_msg *u);
int msgu_share_file_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_share_file_msg *u);


size_t msgu_share_path_size(const struct msgu_share_path_msg *u);
int msgu_share_path_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_path_msg *u);
int msgu_share_path_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_share_path_msg *u);


size_t msgu_node_list_size(const struct msgu_node_list_msg *u);
int msgu_node_list_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_list_msg *u);
int msgu_node_list_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_list_msg *u);


size_t msgu_node_handle_size(const struct msgu_node_handle_msg *u);
int msgu_node_handle_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_handle_msg *u);
int msgu_node_handle_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_handle_msg *u);


size_t msgu_node_read_size(const struct msgu_node_read_msg *u);
int msgu_node_read_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_read_msg *u);
int msgu_node_read_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_read_msg *u);


size_t msgu_node_write_size(const struct msgu_node_write_msg *u);
int msgu_node_write_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_write_msg *u);
int msgu_node_write_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_write_msg *u);


/*
 * print type
 */
void msgu_msgdata_print(char *out, const struct msgu_msgdata *md);


/*
 * free memory
 */
void msgu_msgdata_free(struct msgu_msgdata *md);


/*
 * size required to transfer update
 */
size_t msgu_msgdata_size(int data_type, const union msgu_any_msg *data);


/*
 * read update, return type of update or 0 for incomplete read, and -1 for errors
 */
size_t msgu_msgdata_size_frag(const void *md);
int msgu_msgdata_read_frag(struct msgu_stream *in, struct msgu_fragment *f, void *md);
int msgu_msgdata_write_frag(struct msgu_stream *out, struct msgu_fragment *f, const void *md);


static struct msgu_transfer_fn msgu_msgdata_transfer_fn = {
    .size  = msgu_msgdata_size_frag,
    .read  = msgu_msgdata_read_frag,
    .write = msgu_msgdata_write_frag,
};


#endif

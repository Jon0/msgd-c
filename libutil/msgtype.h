#ifndef LIBUTIL_MSGTYPE_H
#define LIBUTIL_MSGTYPE_H


/*
 * types of messages
 */
enum msgu_msgtype {
    msgtype_wait,
    msgtype_error,

    /*
     * initialise connections
     */
    msgtype_init_local,
    msgtype_init_remote,

    /*
     * network controls
     */
    msgtype_peer_init,
    msgtype_peer_update,
    msgtype_peer_all,
    msgtype_peer_one,

    /*
     * mount controls
     */
    msgtype_mount,
    msgtype_unmount,

    /*
     * share controls
     */
    msgtype_list_shares,
    msgtype_add_share_proc,
    msgtype_add_share_file,
    msgtype_rm_share,

    /*
     * operations on single files
     */
    msgtype_file_stream_read,
    msgtype_file_stream_write,
    msgtype_file_block_read,
    msgtype_file_block_write,

    /*
     * operations on filesystems
     */
    msgtype_path_list,
    msgtype_path_stream_read,
    msgtype_path_stream_write,
    msgtype_path_block_read,
    msgtype_path_block_write,

    /*
     * return types
     */
    msgtype_return_status,
    msgtype_return_share_list,
    msgtype_return_node_list,
};


const char *msgu_msgtype_str(enum msgu_msgtype type);

#endif

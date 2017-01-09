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

    msgtype_mount,
    msgtype_server,
    msgtype_client,
    msgtype_peer_init,
    msgtype_peer_update,
    msgtype_peer_all,
    msgtype_peer_one,
    msgtype_share_proc,
    msgtype_share_file,

    /*
     * share controls
     */
    msgtype_get_shares,
    msgtype_add_share,
    msgtype_rm_share,

    /*
     * operations on single files
     */
    msgtype_file_read,
    msgtype_file_write,

    /*
     * operations filesystems
     */
    msgtype_path_list,
    msgtype_path_read,
    msgtype_path_write,


    /*
     * return types
     */
    msgtype_return_status,
    msgtype_return_share_list,
    msgtype_return_node_list,
};


const char *msgu_msgtype_str(enum msgu_msgtype type);

#endif

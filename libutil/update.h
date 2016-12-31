#ifndef LIBUTIL_UPDATE_H
#define LIBUTIL_UPDATE_H

#include "stream.h"


/*
 * types of messages
 */
enum msg_type_id {
    msg_type_error,
    msg_type_mount,
    msg_type_server,
    msg_type_client,
    msg_type_peer_init,
    msg_type_peer_update,
    msg_type_peer_all,
    msg_type_peer_one,
    msg_type_share_proc,
    msg_type_share_file,
    msg_type_add_share,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_data
};


/*
 * add / remove hosts
 */
struct msgu_host_update {

};


/*
 * adds or removes local shares
 */
struct msgu_share_add_update {

};


/*
 * updates state of a local share
 */
struct msgu_share_update {
    int share_id;
    int operation;
    int data;
};


struct msgu_peer_update {

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
    struct msgu_share_add_update  sh_add;
    struct msgu_share_update      share;
};


#endif

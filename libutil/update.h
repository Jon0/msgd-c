#ifndef LIBUTIL_UPDATE_H
#define LIBUTIL_UPDATE_H

/*
 * add / remove hosts
 */
struct msgu_host_update {

};


/*
 * updates state of a local share
 */
struct msgu_share_update {
    int share_id;
    int operation;
    int data;
};


/*
 * adds or removes local shares
 */
struct msgu_share_list_update {

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

#endif

#ifndef LIBMSG_SHARE_H
#define LIBMSG_SHARE_H

#include <stdlib.h>


struct msg_share_proc {

};


struct msg_share_file {

};


struct msg_share_set {
    struct msg_share_proc *procs;
    struct msg_share_file *files;
    size_t proc_shares;
    size_t file_shares;
};


int msg_share_proc(struct msg_share_set *set);
int msg_share_file(struct msg_share_set *set);


#endif

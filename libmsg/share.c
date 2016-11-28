#include "share.h"


int msg_share_set_init(struct msg_share_set *set) {
    set->procs = malloc(sizeof(struct msg_share_proc) * 32);
    set->files = malloc(sizeof(struct msg_share_files) * 32);
}


int msg_share_proc(struct msg_share_set *set) {

}


int msg_share_file(struct msg_share_set *set) {

}

#include "share.h"


int msg_share_id(void *p) {
    struct msg_share_id *s = (struct msg_share_id *) p;
    return s->id;
}


int msg_share_set_init(struct msg_share_set *set) {
    ep_map_alloc(&set->id_map, msg_share_id, sizeof(struct msg_share_id), 32);
    set->procs = malloc(sizeof(struct msg_share_proc) * 32);
    set->files = malloc(sizeof(struct msg_share_file) * 32);
}


int msg_share_proc(struct msg_share_set *set) {
    int new_id = set->next_id++;
    return new_id;
}


int msg_share_file(struct msg_share_set *set) {
    int new_id = set->next_id++;
    return new_id;
}


int msg_handle_request(struct msg_share_set *set, int share_id) {
    return 0;
}

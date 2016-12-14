#include <stdio.h>

#include "share.h"


int msg_share_id(void *p) {
    struct msg_share_id *s = (struct msg_share_id *) p;
    return s->id;
}


void ep_share_set_init(struct msg_share_set *set) {

}


void ep_share_set_print(struct msg_share_set *set) {

}


size_t ep_share_set_size(struct msg_share_set *set) {
    return 0;
}


size_t ep_share_set_read(struct msg_share_set *set, struct msgu_buffer *buf, size_t offset) {
    return 0;
}


size_t ep_share_set_write(struct msg_share_set *set, struct msgu_buffer *buf) {
    return 0;
}


void msg_share_debug(struct msg_share_server *set) {
    printf("%lu procs, %lu files\n", set->proc_shares, set->file_shares);
}


int msg_share_set_init(struct msg_share_server *set) {
    msgu_map_init(&set->id_map, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct msg_share_id));
    msgu_map_alloc(&set->id_map, 32);
    set->procs = malloc(sizeof(struct msg_share_proc) * 32);
    set->files = malloc(sizeof(struct msg_share_file) * 32);
}


int msg_share_proc(struct msg_share_server *set) {
    int new_id = set->next_id++;
    return new_id;
}


int msg_share_file(struct msg_share_server *set) {
    int new_id = set->next_id++;
    return new_id;
}


int msg_handle_request(struct msg_share_server *set, int share_id) {
    return 0;
}

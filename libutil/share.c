#include <stdio.h>

#include "share.h"


void ep_share_set_init(struct msgu_share_set *set) {

}


void ep_share_set_print(struct msgu_share_set *set) {

}


size_t ep_share_set_size(struct msgu_share_set *set) {
    return 0;
}


size_t ep_share_set_read(struct msgu_share_set *set, struct msgu_stream *s) {
    return 0;
}


size_t ep_share_set_write(struct msgu_share_set *set, struct msgu_stream *s) {
    return 0;
}


void msgu_share_debug(struct msgu_share_map *set) {
    printf("%lu procs, %lu files\n", set->proc_shares, set->file_shares);
}


int msgu_share_set_init(struct msgu_share_map *set) {
    msgu_map_init(&set->id_map, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(struct msgu_share_id));
    msgu_map_alloc(&set->id_map, 32);
    set->procs = malloc(sizeof(struct msg_share_proc) * 32);
    set->files = malloc(sizeof(struct msg_share_file) * 32);
}


int msgu_share_proc(struct msgu_share_map *set) {
    int new_id = set->next_id++;
    return new_id;
}


int msgu_share_file(struct msgu_share_map *set, const struct msgu_string *path) {
    int new_id = set->next_id++;
    struct msgu_share_id share_id;
    share_id.index = set->file_shares++;
    set->files[share_id.index].share_path = *path;
    msgu_map_insert(&set->id_map, &new_id, &share_id);
    return new_id;
}

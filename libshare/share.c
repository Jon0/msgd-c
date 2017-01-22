#include <stdio.h>

#include "share.h"


void msgu_share_debug(struct msgu_share_map *set) {
    printf("%lu procs, %lu files\n", set->proc_shares, set->file_shares);
    for (int i = 0; i < set->proc_shares; ++i) {
        printf("\tproc %d: %s\n", i, set->procs[i].node.node_name.buf);
    }
    for (int i = 0; i < set->file_shares; ++i) {
        printf("\tfile %d: %s\n", i, set->files[i].node.node_name.buf);
    }
}


int msgu_share_set_init(struct msgu_share_map *set) {
    msgu_map_init(&set->id_map, msgu_string_hash, msgu_string_cmp, sizeof(struct msgu_string), sizeof(struct msgu_share_id));
    msgu_map_alloc(&set->id_map, 32);
    set->procs = malloc(sizeof(struct msgu_share_proc) * 32);
    set->files = malloc(sizeof(struct msgu_share_file) * 32);
    set->proc_shares = 0;
    set->file_shares = 0;
}


int msgu_share_proc(struct msgu_share_map *set, const struct msgu_string *name) {
    struct msgu_share_id share_id;
    share_id.type = 1;
    share_id.index = set->proc_shares++;
    struct msgu_share_proc *proc = &set->procs[share_id.index];
    proc->node.node_type = 1;
    proc->node.node_mode = 7;
    msgu_string_copy(&proc->node.node_name, name);
    return msgu_map_insert(&set->id_map, name, &share_id);
}


int msgu_share_file(struct msgu_share_map *set, const struct msgu_string *name) {
    struct msgu_share_id share_id;
    share_id.type = 2;
    share_id.index = set->file_shares++;
    struct msgu_share_file *file = &set->files[share_id.index];
    file->node.node_type = 2;
    file->node.node_mode = 7;
    msgu_string_copy(&file->node.node_name, name);
    msgu_string_copy(&file->share_path, name);
    return msgu_map_insert(&set->id_map, name, &share_id);
}


int msgu_share_get_file(struct msgu_share_map *set, const struct msgu_string *name, struct msgu_share_file *fshare) {
    struct msgu_share_id *sid = msgu_map_get(&set->id_map, name);
    if (sid && sid->type == 2) {
        *fshare = set->files[sid->index];
        return 1;
    }
    else {
        return 0;
    }
}


void msgs_node_list_of_shares(struct msgu_share_map *set, struct msgu_queue *nodes) {
    msgu_queue_init(nodes, &msgu_node_element, sizeof(struct msgu_node));
    msgu_queue_alloc(nodes, set->proc_shares + set->file_shares);
    for (int i = 0; i < set->proc_shares; ++i) {
        msgu_queue_push(nodes, &set->procs[i].node, 1);
    }
    for (int i = 0; i < set->file_shares; ++i) {
        msgu_queue_push(nodes, &set->files[i].node, 1);
    }
}

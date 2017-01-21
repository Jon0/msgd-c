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
    for (int i = 0; i < set->proc_shares; ++i) {
        printf("\tproc %d: %s\n", i, set->procs[i].meta.name.buf);
    }
    for (int i = 0; i < set->file_shares; ++i) {
        printf("\tfile %d: %s\n", i, set->files[i].meta.name.buf);
    }
}


int msgu_share_set_init(struct msgu_share_map *set) {
    msgu_map_init(&set->id_map, msgu_string_hash, msgu_string_cmp, sizeof(struct msgu_string), sizeof(struct msgu_share_id));
    msgu_map_alloc(&set->id_map, 32);
    set->procs = malloc(sizeof(struct msg_share_proc) * 32);
    set->files = malloc(sizeof(struct msg_share_file) * 32);
    set->proc_shares = 0;
    set->file_shares = 0;
}


int msgu_share_proc(struct msgu_share_map *set, const struct msgu_string *name) {
    struct msgu_share_id share_id;
    share_id.index = set->proc_shares++;
    set->procs[share_id.index].meta.name = *name;
    return msgu_map_insert(&set->id_map, name, &share_id);
}


int msgu_share_file(struct msgu_share_map *set, const struct msgu_string *name) {
    struct msgu_share_id share_id;
    share_id.index = set->file_shares++;
    set->files[share_id.index].meta.name = *name;
    return msgu_map_insert(&set->id_map, name, &share_id);
}


int msgu_share_list(struct msgu_share_map *set, struct msgu_stream *out) {
    return 0;
}


int msgu_share_list_dir(struct msgu_share_map *set, struct msgu_stream *out) {
    return 0;
}


void msgs_node_list_of_shares(struct msgu_share_map *set, struct msgu_queue *nodes) {
    msgu_queue_init(nodes, &msgu_node_element, sizeof(struct msgu_node));
    msgu_queue_alloc(nodes, set->proc_shares + set->file_shares);
    struct msgu_node node;
    for (int i = 0; i < set->proc_shares; ++i) {
        node.node_type = 1;
        node.node_mode = 7;
        node.node_name = set->procs[i].meta.name;
        msgu_queue_push(nodes, &node, 1);
    }
    for (int i = 0; i < set->file_shares; ++i) {
        node.node_type = 2;
        node.node_mode = 7;
        node.node_name = set->files[i].meta.name;
        msgu_queue_push(nodes, &node, 1);
    }
}

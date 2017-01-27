#include <stdio.h>

#include "mount.h"


void msgu_mount_map_init(struct msgu_mount_map *m) {
    msgu_map_init(&m->data, msgu_string_hash, msgu_string_cmp, sizeof(struct msgu_string), sizeof(struct msgu_mount_point));
    msgu_map_alloc(&m->data, 32);
}


void msgu_mount_add(struct msgu_mount_map *m, const struct msgu_string *mount_name) {
    struct msgu_string name;
    struct msgu_mount_point mp;
    msgu_string_copy(&name, mount_name);
    msgu_string_copy(&mp.node.node_name, mount_name);
    mp.node.node_type = 2;
    mp.node.node_mode = 7;
    mp.node.node_size = 4096;
    msgu_map_insert(&m->data, &name, &mp);
    printf("mounting: %s\n", mount_name->buf);
}


size_t msgu_mount_map_size(struct msgu_mount_map *m) {
    return msgu_map_size(&m->data);
}


struct msgu_node *msgu_mount_index(struct msgu_mount_map *m, size_t index) {
    struct msgu_mount_point *mp = msgu_map_get_value(&m->data, index);
    return &mp->node;
}


struct msgu_node *msgu_mount_node(struct msgu_mount_map *m, const struct msgu_string *mount_name) {
    struct msgu_mount_point *mp = msgu_map_get(&m->data, mount_name);
    if (mp) {
        return &mp->node;
    }
    else {
        return NULL;
    }
}


int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off) {
    return 0;
}


int msgu_mount_read_response(struct msgu_mount_map *m, struct msgu_string *mount_name) {
    return 0;
}

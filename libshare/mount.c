#include <stdio.h>

#include "mount.h"


void msgu_mount_map_init(struct msgu_mount_map *m) {
    msgu_map_init(&m->data, msgu_string_hash, msgu_string_cmp, sizeof(struct msgu_string), sizeof(struct msgu_mount_point));
    msgu_map_alloc(&m->data, 32);
}


void msgu_mount_add(struct msgu_mount_map *m, int id, const struct msgu_node *node, void *arg) {
    struct msgu_string name;
    struct msgu_mount_point mp;
    msgu_string_copy(&name, &node->node_name);
    mp.id = id;
    mp.arg = arg;
    mp.node = *node;
    msgu_map_insert(&m->data, &name, &mp);
    printf("mounting: %s\n", node->node_name.buf);
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
    struct msgu_string mount_name;
    size_t len = msgu_string_split(&mount_name, 1, path, "/");
    if (len) {
        struct msgu_mount_point *mp = msgu_map_get(&m->data, &mount_name);
        if (mp) {
            return mp->id;
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }
}


int msgu_mount_read_response(struct msgu_mount_map *m, struct msgu_string *mount_name) {
    return 0;
}

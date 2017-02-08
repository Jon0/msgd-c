#include <stdio.h>

#include "mount.h"


hash_t msgu_mount_address_hash(const void *p) {
    const struct msgu_mount_address *addr = p;
    return msgu_string_hash(&addr->host_name) ^ msgu_string_hash(&addr->share_name);
}


int msgu_mount_address_cmp(const void *a, const void *b) {
    const struct msgu_mount_address *addr_a = a;
    const struct msgu_mount_address *addr_b = b;
    return msgu_string_cmp(&addr_a->host_name, &addr_b->host_name) ||
           msgu_string_cmp(&addr_a->share_name, &addr_b->share_name);
}


int msgu_mount_address_path(struct msgu_mount_address *addr, const char *path) {
    struct msgu_string addrstr [2];
    size_t len = msgu_string_split(addrstr, 2, path, "/");
    if (len == 2) {
        addr->host_name = addrstr[0];
        addr->share_name = addrstr[1];
        return 1;
    }
    else {
        return 0;
    }
}


void msgu_mount_map_init(struct msgu_mount_map *m, size_t size) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(struct msgu_mount_address),
        sizeof(struct msgu_mount_point),
    };
    msgu_datatable_init(&m->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
    msgu_datatable_alloc(&m->data, size);
    msgu_datamap_init(&m->id_map, &m->data, 0, msgu_int_hash, msgu_int_cmp);
    msgu_datamap_init(&m->addr_map, &m->data, 1, msgu_mount_address_hash, msgu_mount_address_cmp);
    msgu_datamap_alloc(&m->id_map, size);
    msgu_datamap_alloc(&m->addr_map, size);
}


void msgu_mount_add(struct msgu_mount_map *m, int id, const struct msgu_node *node) {
    struct msgu_string name;
    struct msgu_mount_point mp;
    msgu_string_copy(&name, &node->node_name);
    //mp.id = id;
    mp.node = *node;
    //msgu_map_insert(&m->data, &name, &mp);
    printf("mounting: %s\n", node->node_name.buf);
}


size_t msgu_mount_map_size(struct msgu_mount_map *m) {
    //return msgu_map_size(&m->data);
}


struct msgu_node *msgu_mount_index(struct msgu_mount_map *m, size_t index) {
    void *datarow[3];
    int result = msgu_datatable_get(&m->data, datarow, index);
    if (result < 0) {
        return NULL;
    }
    else {
        struct msgu_mount_point *mp = datarow[2];
        return &mp->node;
    }
}


struct msgu_node *msgu_mount_addr(struct msgu_mount_map *m, const struct msgu_mount_address *ma) {
    struct msgu_mount_point *mp = msgu_mount_get(m, ma);
    if (mp) {
        return &mp->node;
    }
    else {
        return NULL;
    }
}


struct msgu_mount_point *msgu_mount_get(struct msgu_mount_map *m, const struct msgu_mount_address *ma) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->addr_map, ma, datarow);
    if (index < 0) {
        printf("Mount address %s::%s not found\n", ma->host_name.buf, ma->share_name.buf);
        return NULL;
    }
    return datarow[2];
}


int msgu_mount_get_id(struct msgu_mount_map *m, const struct msgu_mount_address *ma) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->addr_map, ma, datarow);
    if (index < 0) {
        printf("Mount address %s::%s not found\n", ma->host_name.buf, ma->share_name.buf);
        return -1;
    }
    return *((int *) datarow[0]);
}


int msgu_mount_open_request(struct msgu_mount_map *m, const char *path) {
    struct msgu_mount_address ma;
    if (msgu_mount_address_path(&ma, path)) {
        return msgu_mount_get_id(m, &ma);
    }
    else {
        return -1;
    }
}


int msgu_mount_open_wait(struct msgu_mount_map *m, struct msgu_string *mount_name) {
    // wait for reply
}


int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off) {
    struct msgu_mount_address ma;
    if (msgu_mount_address_path(&ma, path)) {
        return msgu_mount_get_id(m, &ma);
    }
    else {
        return -1;
    }
}


int msgu_mount_read_wait(struct msgu_mount_map *m, struct msgu_string *mount_name) {
    return 0;
}

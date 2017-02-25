#include <stdio.h>

#include "mount.h"


void msgu_mount_map_init(struct msgu_mount_map *m, size_t size) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(struct msgu_mount_address),
        sizeof(struct msgu_mount_point),
    };
    m->next_id = 1;
    msgu_datatable_init(&m->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
    msgu_datatable_alloc(&m->data, size);
    msgu_datamap_init(&m->id_map, &m->data, 0, msgu_int_hash, msgu_int_cmp);
    msgu_datamap_init(&m->addr_map, &m->data, 1, msgu_mount_address_hash, msgu_mount_address_cmp);
    msgu_datamap_alloc(&m->id_map, size);
    msgu_datamap_alloc(&m->addr_map, size);
}


void msgu_mount_add(struct msgu_mount_map *m, const struct msgu_string *host, const struct msgu_node *node) {
    void *datarow[3];
    printf("mounting: %s\n", node->node_name.buf);
    size_t index = msgu_datatable_emplace(&m->data, datarow);
    *((int *) datarow[0]) = m->next_id++;
    struct msgu_mount_address *addr = datarow[1];
    struct msgu_mount_point *mp = datarow[2];
    msgu_string_copy(&addr->host_name, host);
    msgu_string_copy(&addr->share_name, &node->node_name);
    msgu_datamap_create_key(&m->id_map, index);
    msgu_datamap_create_key(&m->addr_map, index);
    mp->open_handle = -1;
    mp->open_count = 0;
    mp->node = *node;
}


void msgu_mount_set_handle(struct msgu_mount_map *m, const struct msgu_string *host, int hdl) {
    void *datarow[3];
    for (int i = 0; i < m->data.row_count; ++i) {
        msgu_datatable_get(&m->data, datarow, i);
        struct msgu_mount_address *addr = datarow[1];
        struct msgu_mount_point *mp = datarow[2];
        if (msgu_string_compare(host, &addr->host_name) == 0) {
            mp->open_handle = hdl;
        }
    }
}


size_t msgu_mount_map_size(struct msgu_mount_map *m) {
    return m->data.row_count;
}


size_t msgu_mount_by_host(struct msgu_mount_map *m, const struct msgu_string *hostname) {
    size_t count = 0;
    for (int i = 0; i < m->data.row_count; ++i) {
        struct msgu_mount_address *addr = msgu_datatable_get_one(&m->data, i, 1);
        if (msgu_string_compare(hostname, &addr->host_name) == 0) {
            count++;
        }
    }
    printf("host %s, mount count: %lu\n", hostname->buf, count);
    return count;
}


struct msgu_mount_address *msgu_mount_index_addr(struct msgu_mount_map *m, size_t index) {
    void *datarow[3];
    int result = msgu_datatable_get(&m->data, datarow, index);
    if (result < 0) {
        return NULL;
    }
    else {
        return datarow[1];
    }
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


int msgu_mount_exists(struct msgu_mount_map *m, const struct msgu_mount_address *ma) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->addr_map, ma, datarow);
    if (index < 0) {
        return 0;
    }
    else {
        return 1;
    }
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


struct msgu_mount_point *msgu_mount_get(struct msgu_mount_map *m, const struct msgu_mount_address *ma) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->addr_map, ma, datarow);
    if (index < 0) {
        printf("Mount address %s::%s not found\n", ma->host_name.buf, ma->share_name.buf);
        return NULL;
    }
    return datarow[2];
}


struct msgu_mount_point *msgu_mount_get_node(struct msgu_mount_map *m, int id) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->id_map, &id, datarow);
    if (index < 0) {
        printf("Mount id %d not found\n", id);
        return NULL;
    }
    return datarow[2];
}


struct msgu_mount_address *msgu_mount_get_addr(struct msgu_mount_map *m, int id) {
    void *datarow [3];
    int index = msgu_datamap_index(&m->id_map, &id, datarow);
    if (index < 0) {
        printf("Mount id %d not found\n", id);
        return NULL;
    }
    return datarow[1];
}


int msgu_mount_open_request(struct msgu_mount_map *m, const char *path) {
    struct msgu_mount_address ma;
    const char *remain;
    void *datarow [3];
    if (msgu_mount_address_path(&ma, &remain, path) == 2) {
        int index = msgu_datamap_index(&m->addr_map, &ma, datarow);
        if (index < 0) {
            return -1;
        }
        else {
            struct msgu_mount_point *mp = datarow[2];
            mp->open_count++;
            return *((int *) datarow[0]);
        }
    }
    else {
        return -1;
    }
}


int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off) {
    struct msgu_mount_address ma;
    const char *remain;
    if (msgu_mount_address_path(&ma, &remain, path) == 2) {
        return msgu_mount_get_id(m, &ma);
    }
    else {
        return -1;
    }
}


int msgu_mount_attr(struct msgu_mount_map *m, const char *path, struct msgu_node *node) {
    struct msgu_mount_address ma;
    const char *remain;
    int len = msgu_mount_address_path(&ma, &remain, path);
    void *datarow [3];
    int result = 0;
    if (len == 0) {
        msgu_node_dir_init(node, "");
        result = 1;
    }
    else if (len == 1) {
        if (msgu_mount_by_host(m, &ma.host_name) > 0) {
            msgu_node_dir_init(node, ma.host_name.buf);
            result = 1;
        }
    }
    else {
        int index = msgu_datamap_index(&m->addr_map, &ma, datarow);
        if (index >= 0) {
            struct msgu_mount_point *mp = datarow[2];
            *node = mp->node;
            result = 1;
        }
    }
    return result;
}


int msgu_mount_list(struct msgu_mount_map *m, const char *path, struct msgu_vector *nodes) {
    struct msgu_mount_address ma;
    const char *remain;
    int len = msgu_mount_address_path(&ma, &remain, path);
    struct msgu_node newnode;
    msgu_vector_init(nodes, &msgu_node_element, sizeof(struct msgu_node));
    msgu_vector_alloc(nodes, 32);
    void *datarow [3];
    int result = 0;
    if (len == 0) {
        // remove duplicates
        for (int i = 0; i < m->data.row_count; ++i) {
            msgu_datatable_get(&m->data, datarow, i);
            struct msgu_mount_address *addr = datarow[1];
            msgu_node_dir_init(&newnode, addr->host_name.buf);
            msgu_vector_push(nodes, &newnode, 1);
        }
        result = 1;
    }
    else if (len == 1) {
        for (int i = 0; i < m->data.row_count; ++i) {
            msgu_datatable_get(&m->data, datarow, i);
            struct msgu_mount_address *addr = datarow[1];
            struct msgu_mount_point *mp = datarow[2];
            if (msgu_string_compare(&ma.host_name, &addr->host_name) == 0) {
                msgu_vector_push(nodes, &mp->node, 1);
                result = 1;
            }
        }
    }
    return result;
}

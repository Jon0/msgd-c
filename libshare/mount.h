#ifndef LIBSHARE_MOUNT_H
#define LIBSHARE_MOUNT_H

#include <libutil/datatable.h>
#include <libutil/string.h>

#include "node.h"


struct msgu_mount_address {
    struct msgu_string host_name;
    struct msgu_string share_name;
};


int msgu_mount_address_path(struct msgu_mount_address *addr, const char *path);


/*
 * similiar to updates, but only recieved locally
 */
struct msgu_mount_msg {
    int event_type;
    size_t size;
};


/*
 * lists events which require handling
 * TODO should be sorted by name, not id
 */
struct msgu_mount_point {
    struct msgu_node node;
    struct msgu_vector requests;
};


/*
 * resources being used locally
 */
struct msgu_mount_map {
    struct msgu_datatable data;
    struct msgu_datamap   id_map;
    struct msgu_datamap   addr_map;
};


void msgu_mount_map_init(struct msgu_mount_map *m, size_t size);


/*
 *
 */
void msgu_mount_add(struct msgu_mount_map *m, int id, const struct msgu_node *node);


/*
 * return mounts
 */
size_t msgu_mount_map_size(struct msgu_mount_map *m);
struct msgu_node *msgu_mount_index(struct msgu_mount_map *m, size_t index);
struct msgu_node *msgu_mount_addr(struct msgu_mount_map *m, const struct msgu_mount_address *ma);
int msgu_mount_get_id(struct msgu_mount_map *m, const struct msgu_mount_address *ma);
struct msgu_mount_point *msgu_mount_get(struct msgu_mount_map *m, const struct msgu_mount_address *ma);


/*
 * send request and wait for response
 * returns id of mount
 */
int msgu_mount_open_request(struct msgu_mount_map *m, const char *path);
int msgu_mount_open_wait(struct msgu_mount_map *m, struct msgu_string *mount_name);
int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off);
int msgu_mount_read_wait(struct msgu_mount_map *m, struct msgu_string *mount_name);

#endif

#ifndef LIBSHARE_MOUNT_H
#define LIBSHARE_MOUNT_H

#include <libutil/map.h>
#include <libutil/string.h>

#include "node.h"


/*
 * create functions to implement in the server
 */
typedef int (*msgu_mount_read_t)(void *, int, char *, size_t);


struct msgu_mount_fn {
    msgu_mount_read_t read;
};


/*
 * lists events which require handling
 */
struct msgu_mount_point {
    int id;
    struct msgu_node node;
    struct msgu_vector requests;
};


/*
 * resources being used locally
 */
struct msgu_mount_map {
    struct msgu_mount_fn fns;
    struct msgu_map data;
};


void msgu_mount_map_init(struct msgu_mount_map *m);


/*
 *
 */
void msgu_mount_add(struct msgu_mount_map *m, const struct msgu_string *mount_name);


/*
 * return mounts
 */
size_t msgu_mount_map_size(struct msgu_mount_map *m);
struct msgu_node *msgu_mount_index(struct msgu_mount_map *m, size_t index);
struct msgu_node *msgu_mount_node(struct msgu_mount_map *m, const struct msgu_string *mount_name);


/*
 * send request and wait for response
 * returns id of mount
 */
int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off);
int msgu_mount_read_response(struct msgu_mount_map *m, struct msgu_string *mount_name);

#endif

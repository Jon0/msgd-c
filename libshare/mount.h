#ifndef LIBSHARE_MOUNT_H
#define LIBSHARE_MOUNT_H

#include <libutil/datatable.h>
#include <libutil/string.h>

#include "address.h"
#include "node.h"


/*
 * path is empty for single files
 */
struct msgu_mount_handle_map {
    struct msgu_mount_address addr;
    struct msgu_string        path;
    int                       handle_id;
};


/*
 * lists events which require handling
 * TODO should be sorted by name, not id
 */
struct msgu_mount_point {
    int    open_handle;
    size_t open_count;
    struct msgu_node node;
};


/*
 * resources being used locally
 */
struct msgu_mount_map {
    int                   next_id;
    struct msgu_datatable data;
    struct msgu_datamap   id_map;
    struct msgu_datamap   addr_map;
};


void msgu_mount_map_init(struct msgu_mount_map *m, size_t size);


/*
 *
 */
void msgu_mount_add(struct msgu_mount_map *m, const struct msgu_string *host, const struct msgu_node *node);


/*
 * need to find which file the handle is for...
 */
void msgu_mount_set_handle(struct msgu_mount_map *m, const struct msgu_string *host, int hdl);


/*
 * return size of map
 */
size_t msgu_mount_map_size(struct msgu_mount_map *m);


/*
 * find number of mounts from a given host
 */
size_t msgu_mount_by_host(struct msgu_mount_map *m, const struct msgu_string *hostname);


/*
 * find by mount index 0..n
 */
struct msgu_mount_address *msgu_mount_index_addr(struct msgu_mount_map *m, size_t index);
struct msgu_node *msgu_mount_index(struct msgu_mount_map *m, size_t index);


/*
 * find using mount address
 */
int msgu_mount_exists(struct msgu_mount_map *m, const struct msgu_mount_address *ma);
int msgu_mount_get_id(struct msgu_mount_map *m, const struct msgu_mount_address *ma);
struct msgu_node *msgu_mount_addr(struct msgu_mount_map *m, const struct msgu_mount_address *ma);
struct msgu_mount_point *msgu_mount_get(struct msgu_mount_map *m, const struct msgu_mount_address *ma);


/*
 * find using id
 */
struct msgu_mount_point *msgu_mount_get_node(struct msgu_mount_map *m, int id);
struct msgu_mount_address *msgu_mount_get_addr(struct msgu_mount_map *m, int id);


/*
 * send request and wait for response
 * returns id of mount
 */
int msgu_mount_open_request(struct msgu_mount_map *m, const char *path);
int msgu_mount_read_request(struct msgu_mount_map *m, const char *path, size_t size, size_t off);


int msgu_mount_attr(struct msgu_mount_map *m, const char *path, struct msgu_node *node);
int msgu_mount_list(struct msgu_mount_map *m, const char *path, struct msgu_vector *nodes);


#endif

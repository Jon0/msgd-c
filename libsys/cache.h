#ifndef LIBSYS_CACHE_H
#define LIBSYS_CACHE_H

#include <libutil/map.h>
#include <libshare/share.h>


struct msgs_cache_key {
    struct msgu_string remote_name;
    int                handle_id;
};


/*
 * many handle can point to a single resource
 */
struct msgs_cache_handle {
    int resource_index;
    size_t scan_index;
};


/*
 * should only need to open resources once.
 * could use mmap here
 */
struct msgs_cache_resource {
    struct msgu_string share_name;
    int fd;
    char *buffer;
    size_t count;
    size_t alloc;
};


/*
 * opens resources when required,
 * cleans up resources when there are no active handles
 */
struct msgs_file_cache {
    struct msgs_cache_resource *resources;
    struct msgu_share_map *shared;
    struct msgu_map        filemap;
    size_t used_resources;
};


void msgs_file_cache_init(struct msgs_file_cache *c, struct msgu_share_map *shares);
int msgs_file_cache_resource(struct msgs_file_cache *c, const struct msgu_string *share_name);


int msgs_file_handle(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share);
int msgs_path_handle(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share, const struct msgu_string *path);


int msgs_file_read(struct msgs_file_cache *c, struct msgs_cache_key *k, char *buf, size_t count);


#endif

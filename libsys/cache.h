#ifndef LIBSYS_CACHE_H
#define LIBSYS_CACHE_H

#include <libutil/map.h>
#include <libshare/share.h>


struct msgs_cache_key {
    struct msgu_string remote_name;
    int                handle_id;
};


struct msgs_cache_buffer {
    struct msgu_string share_name;
};


struct msgs_file_cache {
    struct msgu_share_map *shared;
    struct msgu_map        filemap;
};


void msgs_file_cache_init(struct msgs_file_cache *c, struct msgu_share_map *shares);


int msgs_file_open(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share);
int msgs_path_open(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share, const struct msgu_string *path);


int msgs_file_read(struct msgs_file_cache *c, struct msgs_cache_key *k, char *buf, size_t count);


#endif

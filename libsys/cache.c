#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cache.h"


hash_t msgu_cache_key_hash(const void *p) {
    const struct msgs_cache_key *k = p;
    return msgu_string_hash(&k->remote_name) ^ msgu_fast_hash(&k->handle_id, sizeof(int));
}


int msgu_cache_key_cmp(const void *a, const void *b) {
    const struct msgs_cache_key *ka = a;
    const struct msgs_cache_key *kb = b;
    return msgu_string_cmp(&ka->remote_name, &kb->remote_name) || (ka->handle_id - kb->handle_id);
}


void msgs_file_cache_init(struct msgs_file_cache *c, struct msgu_share_map *shares) {
    c->shared = shares;
    c->resources = malloc(sizeof(struct msgs_cache_resource) * 128);
    c->used_resources = 0;
    msgu_map_init(&c->filemap, msgu_cache_key_hash, msgu_cache_key_cmp, sizeof(struct msgs_cache_key), sizeof(struct msgs_cache_handle));
    msgu_map_alloc(&c->filemap, 128);
}


int msgs_file_cache_resource(struct msgs_file_cache *c, const struct msgu_string *share_name) {
    struct msgu_share_file fshare;

    // check if the share is already open
    for (int i = 0; i < c->used_resources; ++i) {
        if (msgu_string_compare(&c->resources[i].share_name, share_name) == 0) {
            return i;
        }
    }

    // add new resource, finding file path first
    if (msgu_share_get_file(c->shared, share_name, &fshare)) {
        size_t index = c->used_resources++;
        struct msgs_cache_resource *res = &c->resources[index];
        msgu_string_copy(&res->share_name, share_name);
        res->fd = open(fshare.share_path.buf, O_RDWR);
        res->buffer = malloc(1024);
        memset(res->buffer, 0, 1024);
        res->count = read(res->fd, res->buffer, 1024);
        res->alloc = 1024;
        return index;
    }
    else {
        return -1;
    }
}


int msgs_file_handle(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share) {
    struct msgs_cache_handle hdl;
    hdl.resource_index = msgs_file_cache_resource(c, share);
    hdl.scan_index = 0;
    if (hdl.resource_index != -1) {
        msgu_map_insert(&c->filemap, k, &hdl);
    }
    return 0;
}


int msgs_path_handle(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share, const struct msgu_string *path) {
    return 0;
}


int msgs_file_read(struct msgs_file_cache *c, struct msgs_cache_key *k, char *buf, size_t count) {
    struct msgs_cache_handle *hdl = msgu_map_get(&c->filemap, k);
    if (hdl) {
        struct msgs_cache_resource *res = &c->resources[hdl->resource_index];
        memcpy(buf, &res->buffer[hdl->scan_index], count);
        return count;
    }
    else {
        return 0;
    }
}

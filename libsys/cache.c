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
    msgu_map_init(&c->filemap, msgu_cache_key_hash, msgu_cache_key_cmp, sizeof(struct msgs_cache_key), sizeof(struct msgs_cache_buffer));
    msgu_map_alloc(&c->filemap, 128);
}


int msgs_file_open(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share) {
    return 0;
}


int msgs_path_open(struct msgs_file_cache *c, struct msgs_cache_key *k, const struct msgu_string *share, const struct msgu_string *path) {
    return 0;
}


int msgs_file_read(struct msgs_file_cache *c, struct msgs_cache_key *k, char *buf, size_t count) {
    return 0;
}

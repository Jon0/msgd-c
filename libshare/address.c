#include <string.h>

#include "address.h"


hash_t msgu_address_hash(const void *p) {
    const struct msgu_address *addr = p;
    return msgu_fast_hash(addr->data, addr->len);
}


int msgu_address_cmp(const void *a, const void *b) {
    const struct msgu_address *addr_a = a;
    const struct msgu_address *addr_b = b;
    return msgu_binary_cmp(addr_a->data, addr_b->data, addr_a->len, addr_b->len);
}


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


int msgu_mount_address_path(struct msgu_mount_address *addr, const char **remain, const char *path) {
    struct msgu_string addrstr [2];
    char *dup = strdup(path);
    char *string = dup;
    char *token;
    size_t index = 0;
    size_t bytes = 0;
    while (((token = strsep(&string, "/")) != NULL) && (index < 2)) {
        size_t len = strlen(token);
        bytes += (len + 1);
        if (len > 0) {
            msgu_string_from_static(&addrstr[index], token);
            ++index;
        }
    }
    *remain = &path[bytes];
    free(dup);

    if (index == 0) {
        msgu_string_from_static(&addr->host_name, "");
        msgu_string_from_static(&addr->share_name, "");
    }
    else if (index == 1) {
        addr->host_name = addrstr[0];
        msgu_string_from_static(&addr->share_name, "");
    }
    else {
        addr->host_name = addrstr[0];
        addr->share_name = addrstr[1];
    }
    return index;
}

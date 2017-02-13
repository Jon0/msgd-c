#ifndef LIBUTIL_ADDRESS_H
#define LIBUTIL_ADDRESS_H

#include <stdint.h>

#include <libutil/buffer.h>
#include <libutil/hash.h>
#include <libutil/string.h>


struct msgu_ipc {
    size_t len;
    char *data;
};


struct msgu_ipv4 {
    short port;
    char data [4];
};


struct msgu_ipv6 {
    short port;
    char data [16];
};


/*
 * any type of address
 */
struct msgu_address {
    char   data [128];
    size_t len;
};


hash_t msgu_address_hash(const void *p);
int msgu_address_cmp(const void *a, const void *b);


/*
 * address of a single share
 */
struct msgu_mount_address {
    struct msgu_string host_name;
    struct msgu_string share_name;
};


hash_t msgu_mount_address_hash(const void *p);
int msgu_mount_address_cmp(const void *a, const void *b);
int msgu_mount_address_path(struct msgu_mount_address *addr, const char **remain, const char *path);


enum ep_host_flags {
    ep_host_ipv4 = 0x1,
    ep_host_ipv6 = 0x2
};


/*
 * interface connects to addresses
 * find a default
 */
struct ep_network_interface {

};


#endif

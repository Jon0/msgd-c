#ifndef LIBUTIL_NODE_H
#define LIBUTIL_NODE_H

#include <stdint.h>

#include <libutil/string.h>
#include <libutil/vector.h>


enum msgu_node_type {
    msgnode_stream = 1,
    msgnode_file   = 2,
    msgnode_dir    = 3,
};



/*
 * objects shared on a host
 * nodes can be block devices, sockets or directories
 * nodes can contain attribute metadata
 * to indicate uptime, availability
 */
struct msgu_node {
    uint32_t            node_type;
    uint32_t            node_mode;
    uint32_t            node_size;
    struct msgu_string  node_name;
};


void msgu_node_dir_init(struct msgu_node *node, const char *name);
void msgu_node_file_init(struct msgu_node *node, const char *name, size_t size);


void msgu_node_free(struct msgu_node *node);
int msgu_node_is_dir(const struct msgu_node *node);


int msgu_node_print(char *buf, const struct msgu_node *n);
void msgu_node_list_print(char *buf, const struct msgu_vector *q);


/*
 * standard functions for strings
 */
size_t msgu_node_size_frag(const void *n);
int msgu_node_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *n);
int msgu_node_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *n);
hash_t msgu_node_hash(const void *ns);
int msgu_node_cmp(const void *a, const void *b);


static struct msgu_type msgu_node_element = {
    .memory_size  = sizeof(struct msgu_node),
    .serial_size  = msgu_node_size_frag,
    .read  = msgu_node_read_frag,
    .write = msgu_node_write_frag,
    .hash  = msgu_node_hash,
    .cmp   = msgu_node_cmp,
};


#endif

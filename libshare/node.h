#ifndef LIBUTIL_NODE_H
#define LIBUTIL_NODE_H

#include <stdint.h>

#include <libutil/queue.h>
#include <libutil/string.h>


/*
 * objects shared on a host
 * nodes can be block devices, sockets or directories
 * nodes can contain attribute metadata
 * to indicate uptime, availability
 */
struct msgu_node {
    uint32_t            node_type;
    uint32_t            node_mode;
    struct msgu_string  node_name;
};


int msgu_node_print(char *buf, const struct msgu_node *n);
void msgu_node_list_print(char *buf, const struct msgu_queue *q);


/*
 * standard functions for strings
 */
size_t msgu_node_size_frag(const void *n);
int msgu_node_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *n);
int msgu_node_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *n);
hash_t msgu_node_hash(const void *ns);
int msgu_node_cmp(const void *a, const void *b);


static struct msgu_element msgu_node_element = {
    .size  = msgu_node_size_frag,
    .read  = msgu_node_read_frag,
    .write = msgu_node_write_frag,
    .hash  = msgu_node_hash,
    .cmp   = msgu_node_cmp,
};


#endif

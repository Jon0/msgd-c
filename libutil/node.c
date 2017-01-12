#include "node.h"


size_t msgu_node_size_frag(const void *n) {
    const struct msgu_node *node = n;
    return sizeof(node->node_type) + sizeof(node->node_mode) + msgu_string_size(&node->node_name);
}


int msgu_node_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *n) {
    struct msgu_node *node = n;
    static msgu_frag_read_t msgu_node_read_fns[] = {
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_string_read_frag,
    };
    void *layout[] = {
        &node->node_type,
        &node->node_mode,
        &node->node_name,
    };
    return msgu_read_many(src, f, msgu_node_read_fns, layout, 3);
}


int msgu_node_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *n) {
    const struct msgu_node *node = n;
    static msgu_frag_write_t msgu_node_write_fns[] = {
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &node->node_type,
        &node->node_mode,
        &node->node_name,
    };
    return msgu_write_many(dest, f, msgu_node_write_fns, layout, 3);
}


hash_t msgu_node_hash(const void *n) {
    const struct msgu_node *node = n;
    return 0;
}


int msgu_node_cmp(const void *a, const void *b) {
    return 0;
}

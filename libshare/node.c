#include <stdio.h>

#include "node.h"


int msgu_node_print(char *buf, const struct msgu_node *n) {
    return sprintf(buf, "(%s, type: %d, mode %d)", n->node_name.buf, n->node_type, n->node_mode);
}


void msgu_node_list_print(char *buf, const struct msgu_vector *q) {
    int index = 0;
    for (int i = 0; i < q->size; ++i) {
        const struct msgu_node *node = msgu_array_const(&q->arr, i);
        if (i > 0) {
            index += sprintf(&buf[index], ", ");
        }
        index += msgu_node_print(&buf[index], node);
    }
}


size_t msgu_node_size_frag(const void *n) {
    const struct msgu_node *node = n;
    return sizeof(node->node_type) + sizeof(node->node_mode) + msgu_string_size(&node->node_name);
}


int msgu_node_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *n) {
    struct msgu_node *node = n;
    static msgu_transfer_read_t msgu_node_read_fns[] = {
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
    static msgu_transfer_write_t msgu_node_write_fns[] = {
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
    const struct msgu_node *node_a = a;
    const struct msgu_node *node_b = b;
    return msgu_string_compare(&node_a->node_name, &node_b->node_name);
}

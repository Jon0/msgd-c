#include <stdlib.h>

#include "node.h"


void init_node_buffer(struct node_buffer *buf, int64_t size) {
    buf->dest_buf = malloc(sizeof(struct node_dest) * size);
    buf->dest_count = 0;
    buf->max_index = size;
}


void free_node_buffer(struct node_buffer *buf) {
    free(buf->dest_buf);
    buf->dest_count = 0;
    buf->max_index = 0;
}


void resize_node_buffer(struct node_buffer *buf, int64_t size) {
    struct node_buffer new_buffer;
    init_node_buffer(&new_buffer, size);

    // copy elements

}

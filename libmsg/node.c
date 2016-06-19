#include <stdlib.h>

#include "node.h"


void init_node_buffer(struct node_buffer *buf) {
    size_t init_size = 1024;
    init_update_buffer(&buf->update_buf);
    init_mem_block(&buf->dest_buf, init_size);
    init_mem_block(&buf->src_buf, init_size);
    init_mem_block(&buf->tree_buf, init_size);
}


void free_node_buffer(struct node_buffer *buf) {
    free_update_buffer(&buf->update_buf);
    free_mem_block(&buf->dest_buf);
    free_mem_block(&buf->src_buf);
    free_mem_block(&buf->tree_buf);
}


void update_node_buffer(struct node_buffer *buf) {
    // listen for new hosts


    // for each registered process
    // take new data and apply the default transform

    // update the queue the process polls from
    //

    apply_update_buffer(&buf->update_buf);
}


void register_node_src(struct node_buffer *buf, struct node_src *src) {
    struct node_src *src_array = buf->src_buf.ptr;
    src_array[0] = *src;
}

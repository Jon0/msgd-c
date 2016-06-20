#ifndef UPDATE_H
#define UPDATE_H

#include "mem.h"


/*
 * calculate the required update channels
 */
struct node_update {
    int64_t            in_fd;
    struct mem_block   out_fd;
};


struct node_update_buffer {
    struct mem_block   update_buf;
};


void insert_input(struct node_update_buffer *u, int64_t in_fd);
void remove_input(struct node_update_buffer *u, int64_t in_fd);

void insert_output(struct node_update_buffer *u, int64_t in_fd, int64_t out_fd);
void remove_output(struct node_update_buffer *u, int64_t in_fd, int64_t out_fd);

void init_update_buffer(struct node_update_buffer *u);
void free_update_buffer(struct node_update_buffer *u);
void apply_update_buffer(struct node_update_buffer *u);


#endif

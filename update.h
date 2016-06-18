#ifndef UPDATE_H
#define UPDATE_H


struct node_update {
    int64_t   in_fd;
    int64_t  *out_fd;
    int64_t   out_count;
};


void add_update(struct node_update *u);
void rm_update(struct node_update *u);
void apply_update(struct node_update *u);


#endif

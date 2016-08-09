#include "set.h"


void msg_group_init(struct msg_node_group *g) {
    g->ids = malloc(sizeof(int64_t) * 1024);
    g->size = 0;
    g->avail = 1024;
}


int msg_group_reserve(struct msg_node_group *g, int count) {
    int start = g->size;
    g->size += count;
    return start;
}


void msg_set_init(struct msg_node_set *s) {

}


void msg_subset_name(struct node_attr_set *ns, char *name) {}
void msg_subset_type(struct node_attr_set *ns, char *type) {}

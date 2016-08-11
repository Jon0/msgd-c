#include "set.h"


void msg_group_init(struct msg_node_group *g) {
    ep_multimap_init(&g->groups, sizeof(struct msg_node_set), 1024);
}


int msg_group_reserve(struct msg_node_group *g, int count) {
    return 0;
}


void msg_set_init(struct msg_node_set *s) {

}


void msg_subset_name(struct node_attr_set *ns, char *name) {}
void msg_subset_type(struct node_attr_set *ns, char *type) {}

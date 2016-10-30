#ifndef SET_H
#define SET_H

#include <stdlib.h>
#include <stdint.h>

#include <libutil/hashmap.h>
#include <libutil/node.h>


/*
 * map group ids to a set of node ids
 */
struct msg_node_group {
    struct ep_multimap groups;

};


/*
 * a list of node ids
 */
struct msg_node_set {
    int64_t   sid;
    int64_t  *nid;
    int64_t   count;
};


/*
 * additional attributes
 */
struct node_attr {
    int64_t nid;
    char route [256];
    char type [256];
};


/*
 * a list of nodes and attributes
 */
struct node_attr_set {

};


void msg_group_init(struct msg_node_group *g);
int msg_group_reserve(struct msg_node_group *g, int count);

void msg_set_init(struct msg_node_set *s);


void msg_subset_name(struct node_attr_set *ns, char *name);
void msg_subset_type(struct node_attr_set *ns, char *type);


#endif

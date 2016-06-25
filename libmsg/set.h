#ifndef SET_H
#define SET_H

#include <stdlib.h>
#include <stdint.h>


/*
 * a list of node ids
 */
struct node_id_set {
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





/*
 * memory allocation for node collections
 */
struct set_memory {
    struct node_id_set  *set;
    size_t               used;
    size_t               available;
};




void md_subset_name(struct node_attr_set *ns, char *name);
void md_subset_type(struct node_attr_set *ns, char *type);


#endif

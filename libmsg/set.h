#ifndef SET_H
#define SET_H


/*
 *
 */
struct node_attr {
    int nid;
    char *name;
    char *type;
};


/*
 * a list of nodes
 */
struct node_set {

};


void md_subset_name(struct node_set *ns, char *name);
void md_subset_type(struct node_set *ns, char *type);


#endif

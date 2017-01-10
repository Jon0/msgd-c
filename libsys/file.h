#ifndef FILE_H
#define FILE_H

#include <libutil/node.h>
#include <libutil/tree.h>


/*
 * fill using path
 */
void msgs_node_list_from_path(struct msgu_node_list *list, const char *path);



/*
 * add files or directories to tree
 * return node id
 */
int ep_mount_path(struct ep_tree *t, const char *path);


#endif

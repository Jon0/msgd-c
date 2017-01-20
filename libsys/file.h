#ifndef FILE_H
#define FILE_H

#include <libutil/queue.h>
#include <libutil/tree.h>
#include <libshare/node.h>


/*
 * fill a queue of nodes using specified path
 */
void msgs_node_list_from_path(struct msgu_queue *nodes, const char *path);



/*
 * add files or directories to tree
 * return node id
 */
int ep_mount_path(struct ep_tree *t, const char *path);


#endif

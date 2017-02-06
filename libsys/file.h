#ifndef FILE_H
#define FILE_H

#include <libutil/tree.h>
#include <libutil/vector.h>
#include <libshare/filepath.h>
#include <libshare/node.h>


/*
 * fill a queue of nodes using specified path
 */
void msgs_node_list_from_path(struct msgu_vector *nodes, const char *path);


/*
 * basic filesystem functions
 */
int msgs_path_stat(struct msgu_node *n, const char *path, struct msgu_file_params *param);
int msgs_path_open(struct msgu_resource *r, const char *path, struct msgu_file_params *param);


static struct msgu_file_ops file_ops = {
    .stat = msgs_path_stat,
    .open = msgs_path_open,
};


#endif

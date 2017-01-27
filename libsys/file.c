#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "file.h"


void msgs_node_list_from_path(struct msgu_vector *nodes, const char *path) {
    msgu_vector_init(nodes, &msgu_node_element, sizeof(struct msgu_node));
    msgu_vector_alloc(nodes, 10);
    struct msgu_node node;
    node.node_type = 1;
    node.node_mode = 7;
    msgu_string_from_static(&node.node_name, "filename");
    msgu_vector_push(nodes, &node, 1);
}


int ep_mount_path(struct ep_tree *t, const char *path) {
    // require extra parameters for msgd node_to_sub
    // subid and type labels
    // default to "mount/files"
    // only share files owned by the current user
}


int msgs_path_stat(struct msgu_node *n, const char *path, struct msgu_file_params *param) {
    struct stat fstat;
    int result = stat(path, &fstat);
    if (result == -1) {
        return 0;
    }
    else {
        n->node_type = S_IFMT & fstat.st_mode;
        n->node_mode = ~S_IFMT & fstat.st_mode;
        n->node_size = fstat.st_size;
        msgu_filename(&n->node_name, path);
        return 1;
    }
}


int msgs_path_open(struct msgu_resource *r, const char *path, struct msgu_file_params *param) {
    return 0;
}

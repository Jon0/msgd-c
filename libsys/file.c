#include "file.h"


void msgs_node_list_from_path(struct msgu_queue *nodes, const char *path) {
    msgu_queue_init(nodes, &msgu_node_element, sizeof(struct msgu_node));
    msgu_queue_alloc(nodes, 10);
    struct msgu_node node;
    node.node_type = 1;
    node.node_mode = 7;
    msgu_string_from_static(&node.node_name, "filename");
    msgu_queue_push(nodes, &node, 1);
}


int ep_mount_path(struct ep_tree *t, const char *path) {
    // require extra parameters for msgd node_to_sub
    // subid and type labels
    // default to "mount/files"
    // only share files owned by the current user
}

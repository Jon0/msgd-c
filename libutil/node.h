#ifndef LIBUTIL_NODE_H
#define LIBUTIL_NODE_H

#include <stdint.h>

#include "string.h"


/*
 *
 */
struct msgu_node {
    uint8_t            node_type;
    uint8_t            node_mode;
    struct msgu_string node_name;
};


struct msgu_node_list {
    struct msgu_node *nodes;
    size_t            node_count;
    size_t            allocated_nodes;
};


#endif

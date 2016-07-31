#include <stdio.h>
#include <string.h>

#include "node.h"

void msg_tree_init(struct msg_tree *t) {

}


void msg_tree_add_proc(struct msg_tree *t, char *procname, size_t count) {
    printf("adding %s\n", procname);
}

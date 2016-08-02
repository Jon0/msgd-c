#include <stdio.h>
#include <string.h>

#include "node.h"


void msg_tree_init(struct msg_tree *t, char *hostname) {
    strcpy(t->self.name, hostname);
    printf("[%s] init\n", t->self.name);
}


void msg_tree_add_proc(struct msg_tree *t, char *procname, size_t count) {
    printf("[%s] adding %s\n", t->self.name, procname);
    int index = t->size++;
    memcpy(t->subnodes[index].name, procname, count);

}

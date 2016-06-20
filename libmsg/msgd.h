#ifndef MSGD_H
#define MSGD_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


struct node_state {
    mqd_t queue_id;
    char queue_name [256];
};


void md_init_node(struct node_state *ns, const char *name, int mode);
void md_free_node(struct node_state *ns);

void md_init_pipe(const char *type, const char *name, int mode);

#endif

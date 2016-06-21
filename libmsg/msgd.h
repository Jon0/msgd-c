#ifndef MSGD_H
#define MSGD_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


/*
 * state held by client processes
 */
struct node_state {
    mqd_t queue_id;
    char queue_name [256];
};


void md_init_proc(struct node_state *ns, const char *name, int mode);
void md_free_proc(struct node_state *ns);

void md_publish_node();
void md_subscribe_node();

/*
 * take a copy of the daemons nodes
 */
void md_update(struct node_state *ns);


/*
 * poll new events
 */
void md_poll(struct node_state *ns);

#endif

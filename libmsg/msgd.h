#ifndef MSGD_H
#define MSGD_H

#include <libutil/ipc.h>

/*
 * state held by client processes
 */
struct client_state {
    char                 proc_name [256];
    struct ipc_channel   chan;
};


void md_init_proc(struct client_state *ns, const char *name, int mode);
void md_free_proc(struct client_state *ns);

void md_publish_node(struct client_state *ns);
void md_subscribe_node(struct client_state *ns);

/*
 * take a copy of the daemons nodes
 */
void md_update(struct client_state *ns);


/*
 * poll new events
 */
void md_poll(struct client_state *ns);

#endif

#ifndef MSGD_H
#define MSGD_H

#include <libutil/ipc.h>

#include "set.h"


/*
 * types of messages
 */
enum message_id {
    init,
    join,
    publ,
    subs,
    avail,
    availp,
    avails,
    poll,
    push
};


struct message {
    enum message_id   id;
    char              data [244];
};


/*
 * state held by client processes
 */
struct client_state {
    char                 proc_name [256];
    struct ipc_channel   chan;
};


void md_init_proc(struct client_state *cs, const char *name, int mode);
void md_free_proc(struct client_state *cs);

void md_publish(struct client_state *cs, const struct node_attr_set *ns);
void md_subscribe(struct client_state *cs, const struct node_attr_set *ns);

/*
 * take a copy of the servers nodes
 */
void md_available(struct client_state *cs, struct node_id_set *ns);
void md_published(struct client_state *cs, struct node_id_set *ns);
void md_subscribed(struct client_state *cs, struct node_id_set *ns);

/*
 * poll new events from subscribed nodes
 */
void md_poll(struct client_state *cs);

/*
 * create new events
 */
void md_push(struct client_state *cs);


#endif

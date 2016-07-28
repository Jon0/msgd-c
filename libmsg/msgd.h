#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libutil/endpoint.h>
#include <libutil/thread.h>

#include "set.h"


/*
 * state held by client processes
 */
struct msg_client_state {
    struct ep_table  tb;
    struct ep_thread_pool pool;
    int              epid;
    struct ep_buffer buf;
    size_t           writepos;
    char             proc_name [256];
};


void msg_init_proc(struct msg_client_state *cs, const char *name, int mode);
void msg_free_proc(struct msg_client_state *cs);

void msg_publish(struct msg_client_state *cs, const struct node_attr_set *ns);
void msg_subscribe(struct msg_client_state *cs, const struct node_attr_set *ns);

/*
 * take a copy of the servers nodes
 */
void msg_available(struct msg_client_state *cs, struct node_id_set *ns);
void msg_published(struct msg_client_state *cs, struct node_id_set *ns);
void msg_subscribed(struct msg_client_state *cs, struct node_id_set *ns);

/*
 * poll new events from subscribed nodes
 */
void msg_poll(struct msg_client_state *cs);

/*
 * create new events
 */
void msg_push(struct msg_client_state *cs);


#endif

#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libutil/endpoint.h>

#include "set.h"

/*
 * types of messages
 */
enum msg_type_id {
    msg_type_init,
    msg_type_join,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_push
};


struct msg_block {
    enum msg_type_id  id;
    char              data [244];
};


/*
 * state held by client processes
 */
struct msg_client_state {
    struct ep_table  tb;
    struct ep_buffer buf;
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

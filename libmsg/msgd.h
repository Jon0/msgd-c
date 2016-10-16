#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libutil/endpoint.h>
#include <libutil/thread.h>

#include "set.h"


/*
 * state held by client processes
 * include queue of requests awaiting a response
 */
struct msg_client_state {
    struct ep_table       tb;
    struct ep_thread_pool pool;
    struct ep_tree        tree;
    int                tree_update; // is tree awaiting an update
    int                server_id;   // used to write messages to server
    int                hdlid;
    int                connected;
    char               proc_name [256];
};


int msg_wait(struct msg_client_state *cs, int type);

int msg_connect(struct msg_client_state *cs, const char *addr, short port);
int msg_get_peers(struct msg_client_state *cs);

/*
 * a process connects to the system deamon
 * this should occur after connecting
 */
void msg_create_node(struct msg_client_state *cs, const char *name, int mode);
void msg_free_proc(struct msg_client_state *cs);

/*
 * create a node to send updates
 * return the table element to write updates to
 */
int msg_publish(struct msg_client_state *cs, const char *name, int nodeid);
void msg_subscribe(struct msg_client_state *cs, int nodeid, int subid);


/*
 * take a copy of the servers nodes
 */
int msg_available(struct msg_client_state *cs, struct msg_node_set *ns);
void msg_published(struct msg_client_state *cs, struct msg_node_set *ns);
void msg_subscribed(struct msg_client_state *cs, struct msg_node_set *ns);

/*
 * poll new events from subscribed nodes
 */
void msg_poll(struct msg_client_state *cs);

/*
 * create new events
 */
void msg_push(struct msg_client_state *cs);


#endif

#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libsys/endpoint.h>
#include <libsys/thread.h>

#include "set.h"


struct msg_client_update {
    int type;
    union {
        int tree;
        struct msg_node_update n;
    } data;
};


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
    int                pcount;    // number of used internal ids
    char               proc_name [256];

    // internal node id -> handler id
    struct ep_multimap node_to_hdl;
};




int msg_wait(struct msg_client_state *cs, int type);
int msg_connect(struct msg_client_state *cs, const char *addr, short port);


/*
 * a process connects to the system deamon
 * this should occur after connecting
 */
void msg_register_proc(struct msg_client_state *cs, const char *name, int mode);
void msg_free_proc(struct msg_client_state *cs);

/*
 * create a node to send updates
 * return the table element to write updates to
 */
int msg_publish(struct msg_client_state *cs, const char *name, int mode);
void msg_subscribe(struct msg_client_state *cs, int nodeid, int subid);


/*
 * take a copy of the servers nodes
 */
int msg_available(struct msg_client_state *cs, struct msg_node_set *ns);
void msg_published(struct msg_client_state *cs, struct msg_node_set *ns);
void msg_subscribed(struct msg_client_state *cs, struct msg_node_set *ns);


/*
 * write data to node
 * hdlid is for nodes with distinct output paths
 */
size_t msg_write(struct msg_client_state *cs, int nodeid, int hdlid, char *buf, size_t count);


/*
 * poll new events from subscribed nodes
 * and updates to node structures
 */
void msg_poll(struct msg_client_state *cs, struct msg_client_update *up);


#endif

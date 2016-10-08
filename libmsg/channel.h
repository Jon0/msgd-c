#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/hashmap.h>
#include <libutil/socket.h>
#include <libutil/thread.h>

#include "node.h"

/*
 * a client which requires updates when events occur
 */
struct msg_subscriber {
    int epid;
    int subid;
};


/*
 * TODO replace ep_table with specific functions
 */
struct msg_channel {
    int type;
    int subs;
}


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 * TODO record each socket, and the type of connection (peer or client)
 */
struct msg_server {
    struct ep_table tb;
    struct ep_thread_pool pool;
    struct ep_tree tree;

    // type of socket
    struct ep_map socket_type;

    // the nodes owned by each socket connection
    // int -> int[]
    struct ep_multimap  host_to_tree;

    // nodeid -> struct msg_subscriber
    struct ep_multimap  node_to_sub;
};


void msg_server_printsub(struct msg_server *s);
int msg_node_of_host(struct msg_server *s, int epid);
void msg_server_connect(struct msg_server *s, int epid, int nodeid);
void msg_server_disconnect(struct msg_server *s, int i);
void msg_server_subscribe(struct msg_server *s, int sendnode, int epid, int subid);

void msg_server_accept(struct ep_table *t, int epid, void *in);

void msg_server_recv(int ex, struct ep_event_view *ev);

void msg_server_run(struct msg_server *s, const char *sockpath);

#endif

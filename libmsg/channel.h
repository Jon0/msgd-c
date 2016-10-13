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
    int epid;
    int type;
    int subs;
};


struct msg_known_host {
    char addr [32];
    char name [256];
};


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 * TODO record each socket, and the type of connection (peer or client)
 */
struct msg_server {
    struct ep_table tb;
    struct ep_thread_pool pool;
    struct ep_tree shared_tree;

    // type of socket
    // epid -> struct msg_channel
    struct ep_map socket_type;

    // the nodes owned by each socket connection
    // int -> int[]
    struct ep_multimap  host_to_tree;

    // nodeid -> struct msg_subscriber
    struct ep_multimap  node_to_sub;
};


void msg_server_printsub(struct msg_server *s);
int msg_node_of_host(struct msg_server *s, int epid);
void msg_server_add_client(struct msg_server *s, int epid, int nodeid);
void msg_server_rm_client(struct msg_server *s, int i);
void msg_server_subscribe(struct msg_server *s, int sendnode, int epid, int subid);


/*
 * run generic server
 */
void msg_server_init(struct msg_server *serv, const char *sockpath);
int msg_server_connect(struct msg_server *serv, const char *addr);
void msg_server_run(struct msg_server *serv);
void msg_server_recv(struct msg_server *serv, int epid);
void msg_server_peer_reply(struct msg_server *serv);
void msg_server_client_reply(struct msg_server *serv, int src_epid);
void msg_server_print_debug(struct msg_server *serv);

/*
 * implementing table functions
 */
void msg_server_accept(struct ep_table *t, int epid, void *in);
void msg_server_handler(int ex, struct ep_event_view *ev);


#endif

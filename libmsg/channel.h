#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/hashmap.h>
#include <libsys/network.h>
#include <libsys/socket.h>
#include <libsys/thread.h>

#include "node.h"
#include "protocol.h"


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


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 * TODO record each socket, and the type of connection (peer or client)
 */
struct msg_server {
    struct ep_table tb;
    struct ep_thread_pool pool;


    // type of socket
    // epid -> struct msg_channel
    struct ep_map socket_type;

    // the nodes owned by each socket connection
    // int -> int[]
    struct ep_multimap  host_to_tree;

    // nodeid -> struct msg_subscriber
    struct ep_multimap  node_to_sub;

    struct msg_host *hosts;
    size_t host_count;
};


struct ep_tree *msg_server_self(struct msg_server *s);
int msg_server_init_host(struct msg_server *s);
int msg_server_add_host(struct msg_server *s, const char *addr, const char *name);
void msg_server_printsub(struct msg_server *s);
int msg_node_of_host(struct msg_server *s, int epid);
void msg_server_add_client(struct msg_server *s, int epid, int nodeid);
void msg_server_rm_client(struct msg_server *s, int i);
void msg_server_init_channel(struct msg_server *s, int epid);
void msg_server_subscribe(struct msg_server *s, int sendnode, int epid, int subid);


/*
 * run generic server
 */
void msg_server_init(struct msg_server *serv, const char *sockpath);
int msg_server_connect(struct msg_server *serv, const char *addr);
void msg_server_run(struct msg_server *serv);


/*
 * server responding to events
 */
int msg_server_poll_message(struct ep_buffer *in, struct msg_message *out);
void msg_apply(struct msg_server *serv, int srcid, struct msg_message *m, struct ep_buffer *out);
void msg_server_recv(struct msg_server *serv, int src_epid, struct ep_buffer *buf);
void msg_server_reply(struct msg_server *serv, int src_epid, struct ep_buffer *in, struct ep_channel *out);
void msg_server_print_debug(struct msg_server *serv);

/*
 * implementing table functions
 */
void msg_server_accept(struct ep_table *t, int epid, void *serv);
void msg_server_handler(int ex, struct ep_event_view *ev);


#endif

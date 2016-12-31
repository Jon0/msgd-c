#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/map.h>
#include <libutil/network.h>
#include <libutil/protocol.h>
#include <libutil/share.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>


/*
 * either local-filesystems, local-processes or remote connections
 */
struct msg_connection {
    struct msgs_socket   socket;
    struct msgu_channel  ch;
};


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 */
struct msg_server {
    struct msgu_event_map emap;
    struct msgs_table     tb;

    struct msgs_acceptor local_acc;
    struct msgs_acceptor remote_acc;

    // type of socket
    // epid -> struct msg_connection
    struct msgu_map connections;
    struct msgu_channel_set cs;

    // the nodes owned by each socket connection
    // int -> int[]
    struct msgu_multimap  host_to_tree;

    // nodeid -> struct msg_subscriber
    struct msgu_multimap  node_to_sub;

    struct msg_host_list hosts;

    // maps events to and from local shares
    struct msg_share_map    shares;
};


/*
 * modifications applied to server state
 */
struct msg_delta {
    int                     source_id;
    struct msg_connection  *source;
    int                     update_type;
    union msgu_any_update   update;
};


/*
 * result of applying delta
 */
struct msg_status {
    int error;
};


struct msg_host *msg_server_self(struct msg_server *s);
int msg_server_add_host(struct msg_server *s, const char *addr, const char *name);
void msg_server_printsub(struct msg_server *s);
int msg_node_of_host(struct msg_server *s, int epid);
void msg_server_add_share(struct msg_server *serv, struct msgu_stream *s);
void msg_server_add_client(struct msg_server *s, int epid, int nodeid);
void msg_server_rm_client(struct msg_server *s, int i);
int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket);
struct msg_connection *msg_server_connection(struct msg_server *s, int id);


/*
 * run generic server
 */
void msg_server_init(struct msg_server *serv, const char *sockpath);
int msg_server_connect(struct msg_server *serv, const char *addr);
void msg_server_run(struct msg_server *serv);


/*
 * server responding to events
 */
int msg_server_apply(struct msg_server *serv, const struct msg_delta *delta);
int msg_server_validate(struct msg_server *serv, const struct msg_delta *delta);
int msg_server_modify(struct msg_server *serv, const struct msg_delta *delta, struct msg_status *status);
int msg_server_notify(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status);
int msg_server_reply(struct msg_server *serv, const struct msg_delta *delta, const struct msg_status *status);

//void msg_server_print_debug(struct msg_server *serv);


#endif

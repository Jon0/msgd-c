#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/channel.h>
#include <libutil/map.h>
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
    struct msgu_buffer   read_buf;
    struct msgu_buffer   write_buf;
};


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 * TODO record each socket, and the type of connection (peer or client)
 */
struct msg_server {
    struct msgu_event_map emap;
    struct msgs_table     tb;

    // type of socket
    // epid -> struct msg_connection
    struct msgu_map connections;

    // the nodes owned by each socket connection
    // int -> int[]
    struct msgu_multimap  host_to_tree;

    // nodeid -> struct msg_subscriber
    struct msgu_multimap  node_to_sub;

    struct msg_host_list hosts;

    // local shared objects
    struct msg_share_server shares;
};


/*
 * modifications applied to server state
 */
struct msg_server_delta {
    int type;
};


struct msg_host *msg_server_self(struct msg_server *s);
int msg_server_add_host(struct msg_server *s, const char *addr, const char *name);
void msg_server_printsub(struct msg_server *s);
int msg_node_of_host(struct msg_server *s, int epid);
void msg_server_add_share(struct msg_server *serv, struct msgu_buffer *buf);
void msg_server_add_client(struct msg_server *s, int epid, int nodeid);
void msg_server_rm_client(struct msg_server *s, int i);
int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket);
struct msg_connection *msg_server_connection(struct msg_server *s, int id);
void msg_server_subscribe(struct msg_server *s, int epid, struct msgu_buffer *buf);


/*
 * run generic server
 */
void msg_server_init(struct msg_server *serv, const char *sockpath);
int msg_server_connect(struct msg_server *serv, const char *addr);
void msg_server_run(struct msg_server *serv);


/*
 * server responding to events
 */
void msg_server_apply(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out);
void msg_server_apply_local(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out);
void msg_server_apply_remote(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out);
void msg_server_apply_share(struct msg_server *serv, int srcid, struct msg_message *m, struct msgu_buffer *out);
void msg_server_recv(struct msg_server *serv, int src_epid, struct msgu_buffer *buf);
void msg_server_reply(struct msg_server *serv, int src_epid, struct msgu_buffer *in, struct msgs_socket *out);
void msg_server_print_debug(struct msg_server *serv);


#endif

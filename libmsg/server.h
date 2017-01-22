#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/map.h>
#include <libutil/parser.h>
#include <libshare/host.h>
#include <libshare/network.h>
#include <libshare/share.h>
#include <libsys/file.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>

#include "connection.h"


/*
 * result of applying message
 */
struct msg_status {
    int error;
    int new_handle;
};


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 */
struct msg_server {
    struct msgs_event_map emap;
    struct msgs_table     tb;
    struct msgu_host      self;

    int                  local_acc_id;
    struct msgs_acceptor local_acc;
    int                  remote_acc_id;
    struct msgs_acceptor remote_acc;

    // type of socket
    // epid -> struct msg_connection
    msgs_mutex_t         conn_mutex;
    struct msgu_map      connections;

    // maps events to and from local shares
    struct msgu_share_map shares;
    struct msgs_file_cache cache;
};


/*
 * manage connections
 */
int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket);
int msg_server_close_connection(struct msg_server *s, int id);
struct msg_connection *msg_server_connection(struct msg_server *s, int id);
int msg_server_connection_notify(struct msg_server *serv, int id);


/*
 * run generic server
 */
void msg_server_print_state(struct msg_server *serv);
void msg_server_init(struct msg_server *serv, const char *sockpath);
int msg_server_connect(struct msg_server *serv, const char *addr);
void msg_server_run(struct msg_server *serv);


/*
 * server responding to events
 */
int msg_server_recv(struct msg_server *serv, struct msg_connection *conn, struct msgu_message *msg);
int msg_server_apply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg);
int msg_server_validate(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg);
int msg_server_modify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, struct msg_status *status);
int msg_server_notify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status);
int msg_server_reply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status);


#endif

#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/map.h>
#include <libutil/parser.h>
#include <libshare/mount.h>
#include <libshare/network.h>
#include <libshare/share.h>
#include <libsys/file.h>
#include <libsys/fuse.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>

#include "connection.h"
#include "hostlist.h"


/*
 * result of applying message
 */
struct msg_status {
    int error;
    int new_handle;
    char buf [256];
    int count;
};


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 * allow connection lookup by host name or id
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
    int                  loopback_id;
    struct msgu_map      connections;
    struct msgu_map      hosts;

    // TODO replace above with list:
    struct msg_host_list hostlist;


    // maps events to and from local shares
    struct msgu_share_map shares;
    struct msgu_mount_map mounts;
    struct msgs_file_cache cache;
    struct msgs_fuse_files fuse;
};


/*
 * manage connections
 */
int msg_server_init_connection(struct msg_server *s, struct msgs_socket *socket);
int msg_server_close_connection(struct msg_server *s, int id);
struct msg_connection *msg_server_connection(struct msg_server *s, int id);
struct msg_connection *msg_server_connection_name(struct msg_server *s, const struct msgu_string *hostname);
int msg_server_connection_notify(struct msg_server *serv, int id);


/*
 * manage mounts
 */
void msg_server_init_mount(struct msg_server *serv, const struct msgu_string *host, const struct msgu_string *share);
void msg_server_notify_mount(struct msg_server *serv, int id);


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

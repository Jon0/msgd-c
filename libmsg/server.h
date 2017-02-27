#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/map.h>
#include <libutil/parser.h>
#include <libshare/mount.h>
#include <libshare/network.h>
#include <libshare/share.h>
#include <libsys/file.h>
#include <libsys/network.h>
#include <libsys/table.h>
#include <libsys/thread.h>

#include "connection.h"
#include "hostlist.h"
#include "notify.h"
#include "system.h"


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

    // for creating new connections
    struct msg_system     *external;


    // server state
    struct msgu_host       self;
    int                    msg_id;


    // maps events to and from local shares
    struct msg_host_list hostlist;
    struct msg_notify_map notify;
    struct msgu_share_map shares;
    struct msgu_mount_map mounts;
    struct msgs_file_cache cache;
};


void msg_server_init(struct msg_server *serv, struct msg_system *sys);
int msg_server_accept(struct msg_server *serv, struct msgs_socket *socket);
int msg_server_connect(struct msg_server *serv, const char *addr);


/*
 * print state
 */
void msg_server_print_state(struct msg_server *serv);


/*
 * manage mounts, respond to events
 */
void msg_server_init_mount(struct msg_server *serv, const struct msgu_string *host, const struct msgu_string *share);
void msg_server_mount_pass(struct msg_server *serv, struct msgu_mount_event *e);
void msg_server_notify_mount(struct msg_server *serv, struct msgu_mount_event *e);


/*
 * server responding to socket events
 */
int msg_server_recv(struct msg_server *serv, struct msg_connection *conn, struct msgu_message *msg);
int msg_server_apply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg);
int msg_server_validate(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg);
int msg_server_modify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, struct msg_status *status);
int msg_server_notify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status);
int msg_server_reply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status);


#endif

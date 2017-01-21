#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libutil/parser.h>
#include <libshare/host.h>
#include <libsys/endpoint.h>
#include <libsys/table.h>
#include <libsys/thread.h>

#include "connection.h"


/*
 * Updates passed to processes when polling
 * recieve changes to the directory structures
 * or notification a file has been modified.
 * socket events include the message body,
 * file events include file delta
 */
struct msg_client_update {
    int handler_id;
    int type;
};


/*
 * state held by client processes
 * include queue of requests awaiting a response
 */
struct msg_client_state {
    struct msgs_event_map  emap;
    struct msgs_table      tb;
    struct msg_connection  server;
    int                    server_id;
};


/*
 * init connection to server
 */
int msg_connect(struct msg_client_state *cs, struct msgu_address *addr);
int msg_disconnect(struct msg_client_state *cs);


/*
 * requests to server process
 */
int msg_init_local(struct msg_client_state *cs);
int msg_list_shares(struct msg_client_state *cs);
int msg_create_share(struct msg_client_state *cs, char *path);
int msg_open_share(struct msg_client_state *cs, char *path);
int msg_read(struct msg_client_state *cs, int hdl, size_t count);
int msg_write(struct msg_client_state *cs, int hdl, const char *buf, size_t count);


/*
 * wait for the next recieved message
 */
int msg_wait(struct msg_client_state *cs);


/*
 * poll new events from subscribed nodes
 * and updates to node structures
 */
void msg_poll(struct msg_client_state *cs, struct msg_client_update *up);


#endif

#ifndef MSGD_H
#define MSGD_H

#include <libutil/buffer.h>
#include <libutil/protocol.h>
#include <libsys/endpoint.h>
#include <libsys/table.h>
#include <libsys/thread.h>


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
    struct msgu_event_map emap;
    struct msgs_table     tb;
    struct msgs_socket    server;    // used to write messages to server
    struct msgu_stream    stream;
    int                   server_id;
    int                   hdlid;
    int                   connected;
    int                   pcount;    // number of used internal ids
    char                  proc_name [256];
    struct msgu_buffer    write_buf;

    // internal node id -> handler id
    struct msgu_multimap  node_to_hdl;

    struct msg_host_list  hosts;
};


struct msg_host *msg_client_host(struct msg_client_state *cs);
int msg_connect(struct msg_client_state *cs, struct msgu_address *addr);


/*
 * a process connects to the system deamon
 * this should occur after connecting
 */
void msg_register_proc(struct msg_client_state *cs, const char *name, int mode);
void msg_free_proc(struct msg_client_state *cs);


/*
 * request to server process
 */
int msg_create_share(struct msg_client_state *cs, const char *path);


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

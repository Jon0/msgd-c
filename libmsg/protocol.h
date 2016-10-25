#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>
#include <libsys/event.h>

#include "node.h"

/*
 * TODO send a fixed width type
 */
typedef int32_t msg_int_t;


/*
 * types of messages
 */
enum msg_type_id {
    msg_type_server,
    msg_type_client,
    msg_type_peer_init,
    msg_type_peer_update,
    msg_type_peer_all,
    msg_type_peer_one,
    msg_type_proc_init,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_data
};


struct msg_header {
    enum msg_type_id id;
    int32_t host_id;
    int32_t route_id;
    int32_t state;
    int32_t size;
};


/*
 * messages between client and server
 */
struct msg_message {
    struct msg_header head;
    struct ep_buffer *body;
};


/*
 * per remote host information
 */
struct msg_host {
    int active_id;
    char addr [32];
    char hostname [256];

    // tree containing struct msg_node
    struct ep_tree shared_tree;
};


/*
 * data sent in a subscribe action
 */
struct msg_subscribe {
    int nodeid;
    int subid;
};


int msg_host_init(struct msg_host *h, const char *addr, const char *name);


/*
 * verify a buffer contains a valid message
 */
int msg_invalid_buffer(struct ep_buffer *in);


/*
 * take next message header from incoming buffer
 */
int msg_poll_message(struct ep_buffer *in, struct msg_message *out);


void msg_write_header(struct ep_buffer *b, enum msg_type_id id, int32_t length);


/*
 * request types
 */
void msg_req_peer_init(struct ep_buffer *b, struct msg_host *h);
void msg_req_proc_init(struct ep_buffer *b, const char *msg, size_t count);
void msg_req_avail(struct ep_buffer *b, struct ep_tree *t);
void msg_req_publish(struct ep_buffer *b, const char *name, size_t len, int nodeid);
void msg_req_subscribe(struct ep_buffer *b, int nodeid, int subid);

/*
 * send and recieve objects
 */
void msg_send_self(struct ep_buffer *buf, struct msg_host *h);
void msg_send_peers(struct ep_buffer *buf, struct msg_host *h, size_t host_count);
size_t msg_send_block(struct ep_buffer *buf, int node, int hdl, char *in, size_t count);
void msg_merge_peers(struct ep_buffer *buf, struct msg_host *h, size_t *host_count, size_t host_limit);
void msg_write_host(struct msg_host *in, struct ep_buffer *out);
size_t msg_host_recv(struct ep_buffer *in, struct msg_host *out, size_t offset);
size_t msg_host_merge(struct ep_buffer *in, size_t offset, struct msg_host *h, size_t *host_count);
struct msg_host *msg_host_match(struct msg_host *h, size_t host_count, const char *hostname);


#endif

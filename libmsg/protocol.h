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


struct msg_host_list {
    struct msg_host *ptr;
    size_t host_count;
    size_t host_max;
};


/*
 * data sent in a subscribe action
 */
struct msg_subscribe {
    int nodeid;
    int subid;
};


int msg_host_init(struct msg_host *h, const char *addr, const char *name);
size_t msg_host_read(struct msg_host *out, struct ep_buffer *in, size_t offset);
void msg_host_write(struct msg_host *in, struct ep_buffer *out);
void msg_send_self(struct msg_host *h, struct ep_buffer *buf);

void msg_host_list_debug(struct msg_host_list *h);
int msg_host_list_init(struct msg_host_list *h, size_t max);
int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name);
void msg_merge_peers(struct msg_host_list *h, struct ep_buffer *buf, size_t offset);
size_t msg_host_list_merge(struct msg_host_list *h, struct ep_buffer *in, size_t offset);
struct msg_host *msg_host_match(struct msg_host_list *h, const char *hostname);
void msg_host_list_write(struct msg_host_list *h, struct ep_buffer *buf);


/*
 * verify a buffer contains a valid message
 */
int msg_invalid_buffer(struct ep_buffer *in);


/*
 * take next message header from incoming buffer
 */
int msg_poll_message(struct ep_buffer *in, struct msg_message *out);


/*
 * request types
 */
void msg_write_header(struct ep_buffer *b, enum msg_type_id id, int32_t length);
void msg_req_peer_init(struct ep_buffer *b, struct msg_host *h);
void msg_req_proc_init(struct ep_buffer *b, const char *msg, size_t count);
void msg_req_avail(struct ep_buffer *b, struct ep_tree *t);
void msg_req_publish(struct ep_buffer *b, const char *name, size_t len, int nodeid);
void msg_req_subscribe(struct ep_buffer *b, int nodeid, int subid);

/*
 * send and recieve objects
 */
size_t msg_send_block(struct ep_buffer *buf, int node, int hdl, char *in, size_t count);


#endif

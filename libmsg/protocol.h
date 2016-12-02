#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>
#include <libsys/event.h>

#include "host.h"


/*
 * TODO send a fixed width type
 */
typedef int32_t msg_int_t;


/*
 * types of messages
 */
enum msg_type_id {
    msg_type_error,
    msg_type_mount,
    msg_type_server,
    msg_type_client,
    msg_type_peer_init,
    msg_type_peer_update,
    msg_type_peer_all,
    msg_type_peer_one,
    msg_type_share_proc,
    msg_type_share_file,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_data
};


struct msg_header {
    enum msg_type_id type;
    int32_t host_id;
    int32_t share_id;
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
 * data sent in a subscribe action
 */
struct msg_subscribe {
    int nodeid;
    int subid;
};


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
void msg_req_share(struct ep_buffer *b, const char *path);
void msg_req_peer_init(struct ep_buffer *b, struct msg_host *h);
void msg_req_proc_init(struct ep_buffer *b, const char *msg, size_t count);


/*
 * send and recieve objects
 */
size_t msg_send_block(struct ep_buffer *buf, int node, int hdl, char *in, size_t count);
void msg_send_host(struct msg_host *h, struct ep_buffer *buf);
void msg_send_host_list(struct msg_host_list *h, struct ep_buffer *buf);


#endif

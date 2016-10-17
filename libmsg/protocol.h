#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>
#include <libutil/queue.h>

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
    msg_type_peer,
    msg_type_proc,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_push
};


struct msg_header {
    enum msg_type_id  id;
    int host_id;
    int route_id;
    int state;
    int size;
};


/*
 * messages between client and server
 */
struct msg_message {
    struct msg_header head;
    char              body [256];
};


/*
 * request types
 */
void msg_req_peers(struct ep_buffer *b);
void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count);
void msg_req_avail(struct ep_buffer *b, struct ep_tree *t);
void msg_req_publish(struct ep_buffer *b, const char *name, size_t len, int nodeid);
void msg_req_subscribe(struct ep_buffer *b, int nodeid, int subid);

/*
 * send tree with header
 */
void msg_tree_send(struct ep_tree *tree, struct ep_buffer *out);


#endif

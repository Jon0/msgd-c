#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>
#include <libutil/queue.h>

#include "node.h"

typedef int32_t prot_size_t;

/*
 * types of messages
 */
enum msg_type_id {
    msg_type_init,
    msg_type_join,
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
    int size;
};


/*
 * required to create responses
 */
struct msg_request {
    int               srcid;
    struct ep_event_queue *queue;
    struct ep_buffer *msgbuf;
};


void msg_poll_apply(struct msg_tree *tree, struct msg_request *r);
size_t msg_parse_block(struct msg_tree *tree, struct msg_header *h, char *data, size_t count);


/*
 * request types
 */
void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count);
void msg_req_avail(struct ep_buffer *b);


/*
 * response types
 */
void msg_rsp_avail(struct msg_tree *tree, struct msg_header *h, char *data);

#endif

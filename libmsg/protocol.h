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


struct msg_message {
    struct msg_header head;
    char              body [256];
};


/*
 * required to create responses
 */
struct msg_request {
    struct ep_buffer   *buf;
    struct ep_sink     *src;
};


void msg_poll_apply(struct msg_tree *tree, struct msg_request *r);
void msg_parse(struct msg_tree *tree, struct msg_message *m, struct ep_sink *out);


int msg_read(struct ep_table *t, int epid, struct msg_message *out);


/*
 * request types
 */
void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count);
void msg_req_avail(struct ep_buffer *b);
void msg_req_publish(struct ep_buffer *b, const char *name, size_t len);


/*
 * response types
 */
size_t msg_rsp_avail(struct msg_tree *tree, struct ep_sink *s);

#endif

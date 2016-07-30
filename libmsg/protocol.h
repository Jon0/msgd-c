#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>

#include "node.h"

typedef int32_t prot_size_t;

/*
 * types of messages
 */
enum msg_type_id {
    msg_type_init,
    msg_type_join,
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



void msg_poll_buffer(struct msg_tree *tree, struct ep_buffer *b);
void msg_push_buffer(struct ep_buffer *b, const char *msg, size_t count);
void msg_parse_block(struct msg_tree *tree, struct msg_header *h);


#endif

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <libutil/buffer.h>

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


struct msg_block {
    enum msg_type_id  id;
    int host_id;
    int route_id;
};



void msg_poll_buffer(struct ep_buffer *b);
void msg_push_buffer(struct ep_buffer *b, const char *msg, size_t count);
void msg_parse_block(const char *buf, size_t count);


#endif

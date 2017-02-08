#ifndef LIBUTIL_CHANNEL_H
#define LIBUTIL_CHANNEL_H

#include <libutil/parser.h>

#include "update.h"


/*
 * message type used over sockets
 */
struct msgu_message {
    int32_t                 event_type;
    int32_t                 event_id;
    int32_t                 data_size;
    int32_t                 data_hash;
    struct msgu_msgdata     buf;
};


void msgu_message_free(struct msgu_message *m);
void msgu_message_print(char *buf, const struct msgu_message *m);


/*
 * standard functions for messages
 */
size_t msgu_message_size_frag(const void *m);
int msgu_message_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *m);
int msgu_message_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *m);
hash_t msgu_message_hash(const void *m);
int msgu_message_cmp(const void *a, const void *b);


static struct msgu_type msgu_message_transfer_fn = {
    .memory_size  = sizeof(struct msgu_message),
    .serial_size  = msgu_message_size_frag,
    .read  = msgu_message_read_frag,
    .write = msgu_message_write_frag,
};


#endif

#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/socket.h>


struct msg_channel {
    int source_id;
    int output_id;
};


struct msg_channel_table {
    struct msg_channel *channels;
};



/*
 * complete server state
 */
struct msg_server_state {
    struct ep_table tb;
    struct msg_channel_table c;
};


void msg_parse_input(struct ep_buffer *b);

void msg_server_run(struct msg_server_state *s, const char *sockpath);

#endif

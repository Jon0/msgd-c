#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/socket.h>


/*
 * complete server state
 */
struct msg_server_state {
    struct ep_table tb;
};


void msg_parse_input(struct ep_buffer *b);

void msg_server_run(struct msg_server_state *s);

#endif

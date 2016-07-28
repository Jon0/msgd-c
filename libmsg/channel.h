#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/socket.h>
#include <libutil/thread.h>


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
    struct ep_thread_pool pool;
    struct msg_channel_table c;
};


void msg_parse_input(struct ep_buffer *b);

int msg_server_accept(struct ep_table *t, int *out);

void msg_server_recv(int ex, struct ep_event_view *ev);

void msg_server_run(struct msg_server_state *s, const char *sockpath);

#endif

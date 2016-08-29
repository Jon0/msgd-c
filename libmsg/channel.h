#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/socket.h>
#include <libutil/thread.h>

#include "node.h"


/*
 * complete server state
 */
struct msg_server_state {
    struct ep_table tb;
    struct ep_thread_pool pool;
    struct ep_tree tree;
};

int msg_server_accept(struct ep_table *t, void *in, int *out);

void msg_server_recv(int ex, struct ep_event_view *ev);

void msg_server_run(struct msg_server_state *s, const char *sockpath);

#endif

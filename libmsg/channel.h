#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/hashmap.h>
#include <libutil/socket.h>
#include <libutil/thread.h>

#include "node.h"


/*
 * complete server state
 * includes map from host input ids to tree node ids
 * nodes should be removed when connection is ended
 */
struct msg_server {
    struct ep_table tb;
    struct ep_thread_pool pool;
    struct ep_tree tree;
    struct ep_multimap  host_to_tree;
};

void msg_server_connect(struct msg_server *s, int i);
void msg_server_disconnect(struct msg_server *s, int i);

int msg_server_accept(struct ep_table *t, void *in, int *out);

void msg_server_recv(int ex, struct ep_event_view *ev);

void msg_server_run(struct msg_server *s, const char *sockpath);

#endif

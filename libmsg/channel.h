#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/socket.h>


struct msg_server_state {
    struct ep_table tb;
};


void msg_server_run(struct msg_server_state *s);

#endif

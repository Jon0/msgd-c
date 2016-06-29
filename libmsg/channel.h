#ifndef CHANNEL_H
#define CHANNEL_H

#include <libutil/endpoint.h>


struct msg_channel_state {
    struct ep_table tb;
};


struct msg_mod_state {
    struct msg_channel_state *s;
    void  *ptr;
    char   name [256];
};


/*
 * function for initialising a module
 */
typedef void (*msg_module_fn)(struct msg_mod_state *);


struct msg_mod_private {
    struct msg_mod_state state;
    msg_module_fn enable;
    msg_module_fn disable;
};


void msg_channel_run(struct msg_channel_state *s);

#endif

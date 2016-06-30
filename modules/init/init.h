#ifndef INIT_H
#define INIT_H

#include <libmsg/channel.h>


struct msg_mod_state {
    struct msg_server_state *s;
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



struct init_state {

};


void msgd_enable_module(struct msg_mod_state *ms);
void msgd_disable_module(struct msg_mod_state *ms);


#endif

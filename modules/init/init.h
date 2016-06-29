#ifndef INIT_H
#define INIT_H

#include <libmsg/channel.h>


struct init_state {

};


void msgd_enable_module(struct msg_mod_state *ms);
void msgd_disable_module(struct msg_mod_state *ms);


#endif

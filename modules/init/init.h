#ifndef INIT_H
#define INIT_H

#include <libutil/ipc.h>
#include <libutil/module.h>


struct init_state {
    struct ipc_channel chan;
};


void msgd_init_module(struct module_state *ms);
void msgd_free_module(struct module_state *ms);
void msgd_update_module(struct module_state *ms, struct node_buffer *nb);


#endif

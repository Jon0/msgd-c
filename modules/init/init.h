#ifndef INIT_H
#define INIT_H

#include <module.h>
#include <queue.h>


struct init_state {
    struct queue_state queue;
};


void msgd_init_module(struct module_state *ms);
void msgd_free_module(struct module_state *ms);
void msgd_update_module(struct module_state *ms);


#endif

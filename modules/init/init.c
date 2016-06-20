#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "init.h"


void msgd_init_module(struct module_state *ms) {
    strcpy(ms->name, "msgd-init");
    ms->ptr = malloc(sizeof(struct init_state));
    struct init_state *is = (struct init_state *) ms->ptr;
    open_queue(&is->queue);
}


void msgd_free_module(struct module_state *ms) {
    struct init_state *is = (struct init_state *) ms->ptr;
    close_queue(&is->queue);
    free(ms->ptr);
}


void msgd_update_module(struct module_state *ms) {
    struct init_state *is = (struct init_state *) ms->ptr;
    check_queue(&is->queue);
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "init.h"


void msgd_init_module(struct module_state *ms) {
    strcpy(ms->name, "msgd-init");
    ms->ptr = malloc(sizeof(struct init_state));
    struct init_state *is = (struct init_state *) ms->ptr;
    mdu_ipc_server(&is->chan, ms->name);
}


void msgd_free_module(struct module_state *ms) {
    struct init_state *is = (struct init_state *) ms->ptr;
    mdu_ipc_close(&is->chan);
    free(ms->ptr);
}


void msgd_update_module(struct module_state *ms, struct node_buffer *nb) {
    char buffer [1024];
    struct init_state *is = (struct init_state *) ms->ptr;
    mdu_ipc_recv(&is->chan, buffer, 1024);
}

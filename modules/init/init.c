#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "init.h"


void msgd_enable_module(struct msg_mod_state *ms) {
    // start listener
    ms->ptr = malloc(sizeof(struct init_state));
    strcpy(ms->name, "msgd-init");
}


void msgd_disable_module(struct msg_mod_state *ms) {
    // close listener
    free(ms->ptr);
}

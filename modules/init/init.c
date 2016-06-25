#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libmsg/msgd.h>

#include "init.h"


void msgd_init_module(struct module_state *ms, struct node_buffer *nb) {
    strcpy(ms->name, "msgd-init");
    ms->ptr = malloc(sizeof(struct init_state));
    struct init_state *is = (struct init_state *) ms->ptr;
    mdu_ipc_server(&is->chan, ms->name);

    // setup update functions
    struct node_src src;
    register_node_src(nb, &src);
}


void msgd_free_module(struct module_state *ms) {
    struct init_state *is = (struct init_state *) ms->ptr;
    mdu_ipc_close(&is->chan);
    free(ms->ptr);
}


void msgd_update_module(struct module_state *ms, struct node_buffer *nb) {
    struct message buffer [32];
    struct init_state *is = (struct init_state *) ms->ptr;
    ssize_t recv = mdu_ipc_recv(&is->chan, (char *) buffer, sizeof(struct message) * 32);
    printf("recv %d (%d): %s\n", buffer[0].id, recv, buffer[0].data);

    //
    int id = buffer[0].id;
    switch (id) {
        case init:
        printf("init recieved\n");
        break;
    }
    mdu_ipc_send(&is->chan, (char *) buffer, sizeof(struct message));

}

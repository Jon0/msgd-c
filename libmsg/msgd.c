#include <stdio.h>
#include <string.h>

#include "msgd.h"


void md_init_proc(struct client_state *cs, const char *name, int mode) {
    char buffer [256];
    strcpy(buffer, name);
    printf("connecting to server\n");

    // connect to local ipc
    mdu_ipc_client(&cs->chan, "msgd-init");
    mdu_ipc_send(&cs->chan, buffer, 256);
    printf("sent %s\n", buffer);
}


void md_free_proc(struct client_state *cs) {
    mdu_ipc_close(&cs->chan);
}


void md_publish(struct client_state *cs, const struct node_set *ns) {}
void md_subscribe(struct client_state *cs, const struct node_set *ns) {}
void md_available(struct client_state *cs, struct node_set *ns) {}
void md_published(struct client_state *cs, struct node_set *ns) {}
void md_subscribed(struct client_state *cs, struct node_set *ns) {}
void md_poll(struct client_state *cs) {}

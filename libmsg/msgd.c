#include <stdio.h>
#include <string.h>

#include "msgd.h"


void md_init_proc(struct client_state *cs, const char *name, int mode) {
    struct message msg;
    msg.id = init;
    strcpy(msg.data, name);
    printf("connecting to server\n");

    // connect to local ipc
    mdu_ipc_client(&cs->chan, "msgd-init");
    mdu_ipc_send(&cs->chan, (char *) &msg, sizeof(msg));
    printf("sent %d: %s\n", msg.id, msg.data);
}


void md_free_proc(struct client_state *cs) {
    mdu_ipc_close(&cs->chan);
}


void md_publish(struct client_state *cs, const struct node_attr_set *ns) {}
void md_subscribe(struct client_state *cs, const struct node_attr_set *ns) {}


void md_available(struct client_state *cs, struct node_id_set *ns) {
    struct message msg;
    msg.id = avail;
    mdu_ipc_send(&cs->chan, (char *) &msg, sizeof(msg));
    printf("sent %d: %s\n", msg.id, msg.data);
    mdu_ipc_recv(&cs->chan, (char *) &msg, sizeof(msg));
    printf("recv %d: %s\n", msg.id, msg.data);
}


void md_published(struct client_state *cs, struct node_id_set *ns) {

}

void md_subscribed(struct client_state *cs, struct node_id_set *ns) {

}
void md_poll(struct client_state *cs) {}
void md_push(struct client_state *cs) {}

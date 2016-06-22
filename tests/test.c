#include <string.h>

#include <libmsg/msgd.h>

int main() {
    struct client_state nstate;
    struct ipc_channel chan;
    char buffer [256];
    strcpy(buffer, "connect");

    md_init_proc(&nstate, "test", 0);

    // connect to local ipc
    mdu_ipc_client(&chan, "msgd-init");
    mdu_ipc_send(&chan, buffer, 256);

    md_free_proc(&nstate);
    return 0;
}

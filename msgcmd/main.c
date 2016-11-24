#include <stdio.h>
#include <string.h>

#include <libsys/socket.h>
#include <libmsg/cmdline.h>
#include <libmsg/msgd.h>


void print_usage() {
    printf("msgcmd <address> <command>\n");
}


int main(int argc, char *argv[]) {
    struct msg_client_state nstate;
    int err;

    // try connecting
    struct ep_address addr;
    ep_local(&addr, "msgd-ipc");
    err = msg_connect(&nstate, &addr);
    if (err < 0) {
        printf("cannot connect %s\n", argv[1]);
        return -1;
    }

    // send command

    // cleanup
    msg_free_proc(&nstate);
    return 0;
}

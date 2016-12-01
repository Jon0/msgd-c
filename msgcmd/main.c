#include <stdio.h>
#include <string.h>

#include <libsys/socket.h>
#include <libmsg/cmdline.h>
#include <libmsg/msgd.h>


void print_usage() {
    printf("msgcmd <command>\n");
}


int main(int argc, char *argv[]) {
    struct msg_client_state nstate;
    int err;

    if (argc != 2) {
        print_usage();
        return 0;
    }

    // try connecting
    struct ep_address addr;
    ep_local(&addr, "msgd-ipc");
    err = msg_connect(&nstate, &addr);
    if (err < 0) {
        printf("cannot connect msgd-ipc\n");
        return -1;
    }

    // send command
    msg_create_share(&nstate, argv[1]);

    // cleanup
    msg_free_proc(&nstate);
    return 0;
}

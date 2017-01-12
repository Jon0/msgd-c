#include <stdio.h>
#include <string.h>

#include <libsys/endpoint.h>
#include <libmsg/cmdline.h>
#include <libmsg/msgd.h>


void print_usage() {
    printf("msgcmd <command> <path>\n");
}


void run_command(struct msg_client_state *nstate, char *cmd, int args, char *argv[]) {
    if (strcmp(cmd, "share") == 0) {
        msg_create_share(nstate, argv[0]);
    }
    else if (strcmp(cmd, "list") == 0) {
        msg_list_shares(nstate);
    }
}


int main(int argc, char *argv[]) {
    struct msg_client_state nstate;
    int err;

    if (argc < 2) {
        print_usage();
        return 0;
    }

    // try connecting
    struct msgu_address addr;
    ep_local(&addr, "msgd-ipc");
    err = msg_connect(&nstate, &addr);
    if (err < 0) {
        printf("cannot connect msgd-ipc\n");
        return -1;
    }

    // init connection
    msg_init_local(&nstate);

    // send command
    run_command(&nstate, argv[1], argc - 2, &argv[2]);

    // cleanup
    msg_disconnect(&nstate);
    return 0;
}

#include <stdio.h>
#include <string.h>

#include <libmsg/msgd.h>


void read_command(struct msg_client_state *ns) {
    struct msg_node_set nds;
    char inbuf [256];

    // read input
    // either connect or create a nodes
    scanf("%s", inbuf);

    //
    printf("adding %s\n", inbuf);
    msg_init_proc(ns, inbuf, 0);

    // get available nodes
    // block until result is recieved
    printf("requesting available\n");
    msg_available(ns, &nds);
}


void print_usage() {
    printf("msgd-test <address>\n");
}


int main(int argc, char *argv[]) {
    struct msg_client_state nstate;
    int err;

    // first arg should be the address
    if (argc < 2) {
        print_usage();
        return 0;
    }

    // try connecting
    err = msg_connect(&nstate, argv[1], 2204);
    if (err < 0) {
        printf("cannot connect %s\n", argv[1]);
        return -1;
    }

    // read commnads
    while (1) {
        read_command(&nstate);
    }
    msg_free_proc(&nstate);
    return 0;
}

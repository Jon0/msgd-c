#include <stdio.h>
#include <string.h>

#include <libsys/socket.h>
#include <libmsg/cmdline.h>
#include <libmsg/msgd.h>


void read_command(struct msg_client_state *ns) {
    char inbuf [256];
    int nodebuf;
    size_t len;

    // read input
    // either connect or create a nodes
    printf("enter cmd (reg, share or write)\n");
    msg_read_string(inbuf, sizeof(inbuf));
    if (strcmp(inbuf, "reg") == 0) {
        printf("enter process name\n");
        msg_read_string(inbuf, sizeof(inbuf));
        printf("initalising %s\n", inbuf);
        msg_register_proc(ns, inbuf, 0);

        // get available nodes
        // block until result is recieved
        printf("requesting available\n");
    }
    else if (strcmp(inbuf, "share") == 0) {
        printf("enter share name\n");
        msg_read_string(inbuf, sizeof(inbuf));
        msg_create_share(ns, inbuf);
    }
    else if (strcmp(inbuf, "write") == 0) {
        printf("enter node id\n");
        nodebuf = msg_read_int();
        printf("enter message\n");
        msg_read_string(inbuf, sizeof(inbuf));
        len = strlen(inbuf);
        printf("writing %d (%lu bytes)\n", nodebuf, len);
        msg_write(ns, nodebuf, -1, inbuf, len);
    }
    else {
        printf("cannot match %s\n", inbuf);
    }
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
    struct ep_address addr;
    ep_connect_remote(&addr, argv[1], 2204);
    err = msg_connect(&nstate, &addr);
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

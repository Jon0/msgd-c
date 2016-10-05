#include <stdio.h>
#include <string.h>

#include <libmsg/msgd.h>


void read_string(char *buf, size_t count) {
    fgets(buf, count, stdin);
    buf[strlen(buf) - 1] = 0;
}


int read_int() {
    char *end;
    char inbuf [32];
    read_string(inbuf, sizeof(inbuf));
    return strtol(inbuf, &end, 10);
}


void read_command(struct msg_client_state *ns) {
    struct msg_node_set nds;
    char inbuf [256];
    int nodebuf;

    // read input
    // either connect or create a nodes
    printf("enter cmd (init, add, pub or sub)\n");
    read_string(inbuf, sizeof(inbuf));
    if (strcmp(inbuf, "init") == 0) {
        printf("enter process name\n");
        read_string(inbuf, sizeof(inbuf));
        printf("initalising %s\n", inbuf);
        msg_init_proc(ns, inbuf, 0);

        // get available nodes
        // block until result is recieved
        printf("requesting available\n");
        msg_available(ns, &nds);
    }
    else if (strcmp(inbuf, "add") == 0) {
        printf("enter node name\n");
        read_string(inbuf, sizeof(inbuf));
        printf("adding %s\n", inbuf);
    }
    else if (strcmp(inbuf, "pub") == 0) {
        printf("enter node name\n");
        read_string(inbuf, sizeof(inbuf));
        msg_publish(ns, inbuf, 0);
    }
    else if (strcmp(inbuf, "sub") == 0) {
        printf("enter node id\n");
        nodebuf = read_int();
        printf("subscribing %d\n", nodebuf);
        msg_subscribe(ns, nodebuf, 42);
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

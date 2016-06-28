#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"


void sk_open(char *address) {
    int sd;
    struct sockaddr_un saun, fsaun;
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
    }


    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, address);
    unlink(address);
    int len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (bind(sd, (struct sockaddr *) &saun, len) < 0) {
        perror("server: bind");
        exit(1);
    }
}


void *ep_socket_accept(void *p) {
    struct ep_accept_data *ld = (struct ep_accept_data *) p;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    while (1) {
        // wait for events on the socket
        int r = poll(&ld->event, 1, -1);

        // add entry to endpoint table

        // start new connection threads
        // requires mem allocation for new thread
        int fd = accept(ld->event.fd, (struct sockaddr *) &addr, &len);


    }
}


void *ep_socket_read(void *p) {
    struct ep_read_data *rd = (struct ep_read_data *) p;
    char buf [1024];
    while (1) {
        // wait for events on the socket
        int r = poll(&rd->event, 1, -1);

        // start new connection threads
        // requires mem allocation for new thread
        ssize_t b = read(rd->event.fd, buf, 1024);


    }
}

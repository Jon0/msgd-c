#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"


void ep_unlink(const char *address) {
    unlink(address);
}


void ep_listen_local(struct ep_address *a, const char *address) {

    // intialise address struct with address name
    struct sockaddr_un *saun = (struct sockaddr_un *) &a->data;
    saun->sun_family = AF_UNIX;
    strcpy(saun->sun_path, address);
    a->len = sizeof(saun->sun_family) + strlen(saun->sun_path);
}


void ep_listen_remote(struct ep_address *a, short portnum) {
    struct sockaddr_in *addr = (struct sockaddr_in *) &a->data;
    bzero(addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(portnum);
    a->len = sizeof(struct sockaddr_in);
}


void ep_connect_remote(struct ep_address *a, const char *ip, short portnum) {
    struct sockaddr_in *addr = (struct sockaddr_in *) &a->data;
    bzero(addr, sizeof(struct sockaddr_in));

    struct in_addr buf;
	if (inet_pton(AF_INET, ip, &buf) <= 0) {
        perror("inet_pton");
    }

    addr->sin_family = AF_INET;
    addr->sin_addr = buf;
    addr->sin_port = htons(portnum);
    a->len = sizeof(struct sockaddr_in);
}


void ep_init_acceptor(struct ep_acceptor *a, ep_accept_t af, void *data) {
    struct sockaddr *sa = (struct sockaddr *) &a->addr.data;
    a->create_hdl = af;
    a->data = data;
    a->fd = socket(sa->sa_family, SOCK_STREAM, 0);
    if (a->fd < 0) {
        perror("socket");
        return;
    }

    // bind and listen on the socket
    int err = bind(a->fd, sa, a->addr.len);
    if (err < 0) {
        perror("bind");
        close(a->fd);
        return;
    }
    listen(a->fd, 5);
}


int ep_init_channel(struct ep_channel *c) {
    struct sockaddr *sa = (struct sockaddr *) &c->addr.data;
    c->outcount = 0;
    c->fd = socket(sa->sa_family, SOCK_STREAM, 0);
    if (c->fd < 0) {
        perror("socket");
        return -1;
    }

    // connect to address
    int err = connect(c->fd, sa, c->addr.len);
    if (err == -1) {
        perror("connect");
        close(c->fd);
        return -1;
    }
    return c->fd;
}
